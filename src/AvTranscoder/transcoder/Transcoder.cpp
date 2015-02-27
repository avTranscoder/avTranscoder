#include "Transcoder.hpp"

#include <AvTranscoder/file/util.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>

#include <limits>
#include <algorithm>
#include <sstream>

namespace avtranscoder
{

Transcoder::Transcoder( IOutputFile& outputFile )
	: _outputFile( outputFile )
	, _inputFiles()
	, _streamTranscoders()
	, _streamTranscodersAllocated()
	, _profileLoader( true )
	, _eProcessMethod ( eProcessMethodBasedOnStream )
	, _mainStreamIndex( 0 )
	, _outputDuration( 0 )
{}

Transcoder::~Transcoder()
{
	for( std::vector< InputFile* >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< StreamTranscoder* >::iterator it = _streamTranscodersAllocated.begin(); it != _streamTranscodersAllocated.end(); ++it )
	{
		delete (*it);
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName, const double offset )
{
	// Re-wrap
	if( profileName.length() == 0 )
	{
		// Check filename
		if( filename.length() == 0 )
			throw std::runtime_error( "Can't re-wrap a stream without filename indicated" );

		addRewrapStream( filename, streamIndex );
	}
	// Transcode
	else
	{
		ProfileLoader::Profile& transcodeProfile = _profileLoader.getProfile( profileName );
		add( filename, streamIndex, transcodeProfile, offset );
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName, ICodec& codec, const double offset )
{
	// Re-wrap
	if( profileName.length() == 0 )
	{
		// Check filename
		if( filename.length() == 0 )
			throw std::runtime_error( "Can't re-wrap a stream without filename indicated" );
		
		addRewrapStream( filename, streamIndex );
	}
	// Transcode
	else
	{
		ProfileLoader::Profile& transcodeProfile = _profileLoader.getProfile( profileName );
		add( filename, streamIndex, transcodeProfile, codec, offset );
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, ProfileLoader::Profile& profile, const double offset )
{
	// Check filename
	if( ! filename.length() )
		throw std::runtime_error( "Can't transcode a stream without filename indicated" );

	addTranscodeStream( filename, streamIndex, -1, profile, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, ProfileLoader::Profile& profile, ICodec& codec, const double offset )
{
	// Generator
	if( ! filename.length() )
	{
		addDummyStream( profile, codec );
	}
	// Transcode
	else
	{
		addTranscodeStream( filename, streamIndex, -1, profile, offset );
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, const double offset )
{
	// No subStream selected
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileName, offset );
		return;
	}

	if( profileName.length() == 0 )
	{
		// Re-wrap
		if( subStreamIndex < 0 )
		{
			addRewrapStream( filename, streamIndex );
		}
		// Transcode (transparent for the user)
		else
		{
			addTranscodeStream( filename, streamIndex, subStreamIndex, offset );
		}
	}
	// Transcode
	else
	{
		ProfileLoader::Profile& transcodeProfile = _profileLoader.getProfile( profileName );
		add( filename, streamIndex, subStreamIndex, transcodeProfile, offset );
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, ICodec& codec, const double offset )
{
	// No subStream selected
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileName, codec );
		return;
	}

	// Re-wrap
	if( profileName.length() == 0 )
	{
		// Re-wrap
		if( subStreamIndex < 0 )
		{
			addRewrapStream( filename, streamIndex );
		}
		// Transcode (transparent for the user)
		else
		{
			addTranscodeStream( filename, streamIndex, subStreamIndex, offset );
		}
	}
	// Transcode
	else
	{
		ProfileLoader::Profile& transcodeProfile = _profileLoader.getProfile( profileName );
		add( filename, streamIndex, subStreamIndex, transcodeProfile, codec, offset );
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, ProfileLoader::Profile& profile, const double offset )
{
	// No subStream selected
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profile, offset );
		return;
	}

	// Check filename
	if( ! filename.length() )
		throw std::runtime_error( "Can't transcode a stream without filename indicated" );

	addTranscodeStream( filename, streamIndex, subStreamIndex, profile, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, ProfileLoader::Profile& profile, ICodec& codec, const double offset )
{
	// No subStream selected
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profile );
		return;
	}

	// Generator
	if( ! filename.length() )
	{
		addDummyStream( profile, codec );
		return;
	}

	addTranscodeStream( filename, streamIndex, subStreamIndex, profile, offset );
}

void Transcoder::add( StreamTranscoder& stream )
{
	_streamTranscoders.push_back( &stream );
}

void Transcoder::preProcessCodecLatency()
{
	for( size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex )
	{
		std::stringstream os;
		os << "Init stream " << streamIndex;
		Logger::debug( os.str() );
		_streamTranscoders.at( streamIndex )->preProcessCodecLatency();
	}
}

bool Transcoder::processFrame()
{
	if( _streamTranscoders.size() == 0 )
		return false;

	for( size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex )
	{
		std::stringstream os;
		os << "Process stream " << streamIndex << "/" << ( _streamTranscoders.size() - 1 );
		Logger::debug( os.str() );

		bool streamProcessStatus = _streamTranscoders.at( streamIndex )->processFrame();
		if( ! streamProcessStatus )
		{
			_streamTranscoders.clear();
			return false;
		}
	}
	return true;
}

void Transcoder::process( IProgress& progress )
{
	if( _streamTranscoders.size() == 0 )
		throw std::runtime_error( "missing input streams in transcoder" );

	manageSwitchToGenerator();

	Logger::info( "Start process" );

	_outputFile.beginWrap();

	preProcessCodecLatency();

	double outputDuration = getOutputDuration();

	std::stringstream os;
	size_t frame = 0;
	bool frameProcessed = true;
	while( frameProcessed )
	{
		os << "Process frame " << frame;
		Logger::info( os.str() );
		os.str( "" );

		frameProcessed =  processFrame();

		double progressDuration = _outputFile.getStream( 0 ).getStreamDuration();

		// check progressDuration
		if( progressDuration > outputDuration )
			break;

		// check if JobStatusCancel
		if( progress.progress( ( progressDuration > outputDuration ) ? outputDuration : progressDuration, outputDuration ) == eJobStatusCancel )
			break;

		++frame;
	}

	_outputFile.endWrap();

	Logger::info( "End of process" );
}

void Transcoder::setProcessMethod( const EProcessMethod eProcessMethod, const size_t indexBasedStream, const double outputDuration )
{
	_eProcessMethod	= eProcessMethod;
	_mainStreamIndex = indexBasedStream;
	_outputDuration = outputDuration;
}

void Transcoder::addRewrapStream( const std::string& filename, const size_t streamIndex )
{
	std::stringstream os;
	os << "Add rewrap stream from file '" << filename << "' / index=" << streamIndex;
	Logger::info( os.str() );

	InputFile* referenceFile = addInputFile( filename, streamIndex );

	_streamTranscodersAllocated.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile ) );
	_streamTranscoders.push_back( _streamTranscodersAllocated.back() );
}

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const double offset )
{
	// Get profile from input file
	InputFile* referenceFile = addInputFile( filename, streamIndex );
	ProfileLoader::Profile profile = getProfileFromFile( *referenceFile, streamIndex );

	// override channels parameter to manage demultiplexing
	ProfileLoader::Profile::iterator it = profile.find( constants::avProfileChannel ); 
	if( it != profile.end() )
		it->second = "1";

	addTranscodeStream( filename, streamIndex, subStreamIndex, profile, offset );
}

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const int subStreamIndex, ProfileLoader::Profile& profile, const double offset )
{
	// Add profile
	_profileLoader.loadProfile( profile );

	std::stringstream os;
	os << "Add transcode stream from file '" << filename << "' / index=" << streamIndex << " / channel=";
	if( subStreamIndex < 0 )
		os << "all";
	else
		os << subStreamIndex;
	os << " / encodingProfile=" << profile.at( constants::avProfileIdentificatorHuman ) << " / offset=" << offset << "s";
	Logger::info( os.str() );

	// Add input file
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	switch( referenceFile->getStream( streamIndex ).getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO:
		case AVMEDIA_TYPE_AUDIO:
		{
			_streamTranscodersAllocated.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile, subStreamIndex, offset ) );
			_streamTranscoders.push_back( _streamTranscodersAllocated.back() );
			break;
		}
		case AVMEDIA_TYPE_DATA:
		case AVMEDIA_TYPE_SUBTITLE:
		case AVMEDIA_TYPE_ATTACHMENT:
		default:
		{
			throw std::runtime_error( "unsupported media type in transcode setup" );
		}
	}
}

void Transcoder::addDummyStream( const ProfileLoader::Profile& profile, const ICodec& codec )
{
	// Add profile
	_profileLoader.loadProfile( profile );

	std::stringstream os;
	os << "Add generated stream with codec '" << codec.getCodecName() << "' / encodingProfile=" << profile.at( constants::avProfileIdentificatorHuman );
	Logger::info( os.str() );

	_streamTranscodersAllocated.push_back( new StreamTranscoder( codec, _outputFile, profile ) );
	_streamTranscoders.push_back( _streamTranscodersAllocated.back() );
}

InputFile* Transcoder::addInputFile( const std::string& filename, const size_t streamIndex )
{
	InputFile* referenceFile = NULL;

	for( std::vector< InputFile* >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		if( ( (*it)->getFilename() == filename ) &&
			( ! (*it)->getStream( streamIndex ).isActivated() ) )
		{
			referenceFile = (*it);
			break;
		}
	}

	if( ! referenceFile )
	{
		std::string msg( "New instance of InputFile from '");
		msg += filename;
		msg += "'";
		Logger::debug( msg );

		_inputFiles.push_back( new InputFile( filename ) );
		referenceFile = _inputFiles.back();
	}

	referenceFile->activateStream( streamIndex );

	return referenceFile;
}

ProfileLoader::Profile Transcoder::getProfileFromFile( InputFile& inputFile, const size_t streamIndex )
{
	NoDisplayProgress progress;
	inputFile.analyse( progress, eAnalyseLevelHeader );

	const VideoProperties* videoProperties = NULL;
	const AudioProperties* audioProperties = NULL;
	switch( inputFile.getStream( streamIndex ).getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO:
		{
			videoProperties = &inputFile.getProperties().getVideoPropertiesWithStreamIndex( streamIndex );
			break;
		}
		case AVMEDIA_TYPE_AUDIO:
		{
			audioProperties = &inputFile.getProperties().getAudioPropertiesWithStreamIndex( streamIndex );
			break;
		}
		default:
			break;
	}

	// common fileds in profile types
	ProfileLoader::Profile profile;
	profile[ constants::avProfileIdentificator ] = "profileFromInput";
	profile[ constants::avProfileIdentificatorHuman ] = "profile from input";

	// video
	if( videoProperties != NULL )
	{
		profile[ constants::avProfileType ] = avtranscoder::constants::avProfileTypeVideo;
		profile[ constants::avProfileCodec ] = videoProperties->getCodecName();
		profile[ constants::avProfilePixelFormat ] = videoProperties->getPixelProperties().getPixelFormatName();
		std::stringstream ss;
		ss << videoProperties->getFps();
		profile[ constants::avProfileFrameRate ] = ss.str();
		profile[ constants::avProfileWidth ] = videoProperties->getWidth();
		profile[ constants::avProfileHeight ] = videoProperties->getHeight();
	}
	// audio
	else if( audioProperties != NULL )
	{
		profile[ constants::avProfileType ] = avtranscoder::constants::avProfileTypeAudio;
		profile[ constants::avProfileCodec ] = audioProperties->getCodecName();
		profile[ constants::avProfileSampleFormat ] = audioProperties->getSampleFormatName();
		std::stringstream ss;
		ss << audioProperties->getSampleRate();
		profile[ constants::avProfileSampleRate ] = ss.str();
		ss.str( "" );
		ss << audioProperties->getChannels();
		profile[ constants::avProfileChannel ] = ss.str();
	}

	return profile;
}

double Transcoder::getStreamDuration( size_t indexStream ) const
{
	return _streamTranscoders.at( indexStream )->getDuration();
}

double Transcoder::getMinTotalDuration() const
{
	double minTotalDuration = std::numeric_limits<double>::max();
	
	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		minTotalDuration = std::min( getStreamDuration( i ), minTotalDuration );
	}
	return minTotalDuration;
}

double Transcoder::getMaxTotalDuration() const
{
	double maxTotalDuration = 0;
	
	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		maxTotalDuration = std::max( getStreamDuration( i ), maxTotalDuration );
	}
	return maxTotalDuration;
}

double Transcoder::getOutputDuration() const
{
	switch( _eProcessMethod )
	{
		case eProcessMethodShortest :
			return getMinTotalDuration();
		case eProcessMethodLongest :
			return getMaxTotalDuration();
		case eProcessMethodBasedOnStream :
			return getStreamDuration( _mainStreamIndex );
		case eProcessMethodBasedOnDuration :
			return _outputDuration;
		case eProcessMethodInfinity :
			return std::numeric_limits<double>::max();
		default:
			return getMaxTotalDuration();
	}	
}

void Transcoder::manageSwitchToGenerator()
{
	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		switch( _eProcessMethod )
		{
			case eProcessMethodShortest :
				if( _streamTranscoders.at( i )->getDuration() == getMinTotalDuration() )
					_streamTranscoders.at( i )->canSwitchToGenerator( false );
				else
					_streamTranscoders.at( i )->canSwitchToGenerator( true );
				break;
			case eProcessMethodLongest :
				if( _streamTranscoders.at( i )->getDuration() == getMaxTotalDuration() )
					_streamTranscoders.at( i )->canSwitchToGenerator( false );
				else
					_streamTranscoders.at( i )->canSwitchToGenerator( true );
				break;
			case eProcessMethodBasedOnStream :
				if( i != _mainStreamIndex )
					_streamTranscoders.at( i )->canSwitchToGenerator( true );
				else
					_streamTranscoders.at( i )->canSwitchToGenerator( false );
				break;
			case eProcessMethodBasedOnDuration :
				if( _streamTranscoders.at( i )->getDuration() > _outputDuration )
					_streamTranscoders.at( i )->canSwitchToGenerator( false );
				else
					_streamTranscoders.at( i )->canSwitchToGenerator( true );
				break;
			case eProcessMethodInfinity :
				_streamTranscoders.at( i )->canSwitchToGenerator( true );
				break;
		}
	}
}

}
