#include "Transcoder.hpp"

#include <AvTranscoder/file/util.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>

#include <limits>
#include <iostream>
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
	, _verbose( false )
{
	// Initialize the OutputFile
	_outputFile.setup();

	// Print no output from ffmpeg
	av_log_set_level( AV_LOG_QUIET );
}

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
		if( _verbose )
			std::cout << "Add re-wrap stream" << std::endl;

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
		if( _verbose )
			std::cout << "Add re-wrap stream" << std::endl;

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

	if( _verbose )
		std::cout << "add transcoding stream" << std::endl;
	addTranscodeStream( filename, streamIndex, -1, profile, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, ProfileLoader::Profile& profile, ICodec& codec, const double offset )
{
	// Generator
	if( ! filename.length() )
	{
		if( _verbose )
			std::cout << "Add generated stream" << std::endl;
		addDummyStream( profile, codec );
	}
	// Transcode
	else
	{
		if( _verbose )
			std::cout << "Add transcoded stream" << std::endl;
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
			if( _verbose )
				std::cout << "Add re-wrap stream" << std::endl;
			addRewrapStream( filename, streamIndex );
			return;
		}
		// Transcode (transparent for the user)
		else
		{
			if( _verbose )
				std::cout << "Add transcoded stream (because of demultiplexing)" << std::endl;
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
			if( _verbose )
				std::cout << "Add re-wrap stream" << std::endl;
			addRewrapStream( filename, streamIndex );
			return;
		}
		// Transcode (transparent for the user)
		else
		{
			if( _verbose )
				std::cout << "Add transcoded stream (because of demultiplexing)" << std::endl;
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

	if( _verbose )
		std::cout << "Add transcoded for substream " << subStreamIndex << std::endl;
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
		if( _verbose )
			std::cout << "Add generated stream" << std::endl;
		addDummyStream( profile, codec );
		return;
	}

	if( _verbose )
		std::cout << "Add transcoded stream for substream " << subStreamIndex << std::endl;
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
		if( _verbose )
			std::cout << "init stream " << streamIndex << std::endl;
		_streamTranscoders.at( streamIndex )->preProcessCodecLatency();
	}
}

bool Transcoder::processFrame()
{
	if( _streamTranscoders.size() == 0 )
		return false;

	if( _verbose )
		std::cout << "process frame" << std::endl;

	for( size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex )
	{
		if( _verbose )
			std::cout << "process stream " << streamIndex << "/" << _streamTranscoders.size() - 1 << std::endl;

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

	if( _verbose )
		std::cout << "begin transcoding" << std::endl;

	_outputFile.beginWrap();

	preProcessCodecLatency();

	double outputDuration = getOutputDuration();

	size_t frame = 0;
	bool frameProcessed = true;
	while( frameProcessed )
	{
		if( _verbose )
			std::cout << "process frame " << frame << std::endl;

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

	if( _verbose )
		std::cout << "end of transcoding" << std::endl;

	_outputFile.endWrap();
}

void Transcoder::setProcessMethod( const EProcessMethod eProcessMethod, const size_t indexBasedStream, const double outputDuration )
{
	_eProcessMethod	= eProcessMethod;
	_mainStreamIndex = indexBasedStream;
	_outputDuration = outputDuration;
}

void Transcoder::setVerbose( bool verbose )
{
	_verbose = verbose;
	for( std::vector< StreamTranscoder* >::iterator it = _streamTranscoders.begin(); it != _streamTranscoders.end(); ++it )
	{
		(*it)->setVerbose( _verbose );
	}

	// Print stuff which is only useful for ffmpeg developers.
	if( _verbose )
		av_log_set_level( AV_LOG_DEBUG );
}

void Transcoder::addRewrapStream( const std::string& filename, const size_t streamIndex )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	_streamTranscodersAllocated.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile ) );
	_streamTranscoders.push_back( _streamTranscodersAllocated.back() );
}

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const size_t subStreamIndex, const double offset )
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

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const size_t subStreamIndex, ProfileLoader::Profile& profile, const double offset )
{
	// Add profile
	_profileLoader.loadProfile( profile );

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

	if( _verbose )
	{
		if( profile.find( constants::avProfileType )->second == constants::avProfileTypeVideo )
			std::cout << "add a generated video stream" << std::endl;
		else if( profile.find( constants::avProfileType )->second == constants::avProfileTypeAudio )
			std::cout << "add a generated audio stream" << std::endl;
	}

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
		if( _verbose )
			std::cout << "new InputFile for " << filename << std::endl;
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
		ss.clear();
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
