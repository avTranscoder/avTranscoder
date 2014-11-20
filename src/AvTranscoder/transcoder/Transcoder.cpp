#include "Transcoder.hpp"

#include <AvTranscoder/essenceStream/GeneratorAudio.hpp>
#include <AvTranscoder/essenceStream/GeneratorVideo.hpp>

#include <AvTranscoder/progress/NoDisplayProgress.hpp>

#include <limits>
#include <iostream>
#include <algorithm>
#include <sstream>

namespace avtranscoder
{

Transcoder::Transcoder( OutputFile& outputFile )
	: _outputFile( outputFile )
	, _inputFiles()
	, _streamTranscoders()
	, _streamTranscodersAllocated()
	, _profileLoader( true )
	, _eProcessMethod ( eProcessMethodLongest )
	, _mainStreamIndex( 0 )
	, _verbose( false )
{
	_outputFile.setup();
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
	for( std::vector< IInputEssence* >::iterator it = _internalEssences.begin(); it != _internalEssences.end(); ++it )
	{
		delete (*it);
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName, const size_t offset )
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

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName, ICodec& codec, const size_t offset )
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

void Transcoder::add( const std::string& filename, const size_t streamIndex, ProfileLoader::Profile& profile, const size_t offset )
{
	// Add profile if new
	_profileLoader.update( profile );

	// Check filename
	if( ! filename.length() )
		throw std::runtime_error( "Can't transcode a stream without filename indicated" );

	if( _verbose )
		std::cout << "add transcoding stream" << std::endl;
	addTranscodeStream( filename, streamIndex, -1, profile, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, ProfileLoader::Profile& profile, ICodec& codec, const size_t offset )
{
	// Add profile if new
	_profileLoader.update( profile );

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

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, const size_t offset )
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

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, ICodec& codec, const size_t offset )
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

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, ProfileLoader::Profile& profile, const size_t offset )
{
	// Add profile if new
	_profileLoader.update( profile );

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

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, ProfileLoader::Profile& profile, ICodec& codec, const size_t offset )
{
	// Add profile if new
	_profileLoader.update( profile );
	
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

void Transcoder::init()
{
	for( size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex )
	{
		if( _verbose )
			std::cout << "init stream " << streamIndex << std::endl;
		_streamTranscoders.at( streamIndex )->init();
	}
}

bool Transcoder::processFrame()
{
	if( _streamTranscoders.size() == 0 )
	{
		return false;
	}

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
	size_t frame = 0;

	if( ! _streamTranscoders.size() )
	{
		throw std::runtime_error( "missing input streams in transcoder" );
	}
	
	if( _verbose )
		std::cout << "begin transcoding" << std::endl;
	
	_outputFile.beginWrap();

	double totalDuration = std::numeric_limits<double>::max();
	switch( _eProcessMethod )
	{
		case eProcessMethodShortest :
			totalDuration = getMinTotalDuration();
			break;
		case eProcessMethodLongest :
			totalDuration = getMaxTotalDuration();
			break;
		case eProcessMethodBasedOnStream :
			totalDuration = getStreamDuration( _mainStreamIndex );
			break;
		case eProcessMethodInfinity :
			totalDuration = std::numeric_limits<double>::max();
			break;
	}

	if( _verbose )
		av_log_set_level( AV_LOG_DEBUG );

	while( 1 )
	{
		if( _verbose )
			std::cout << "process frame " << frame << std::endl;
		if( ! processFrame() )
			break;
		
		if( progress.progress( 1 / 25. * ( frame ), totalDuration ) == eJobStatusCancel )
		{
			break;
		}

		++frame;
	}

	if( _verbose )
		std::cout << "end of transcoding" << std::endl;

	_outputFile.endWrap();
}

void Transcoder::setProcessMethod( const EProcessMethod eProcessMethod, const size_t indexBasedStream )
{
	_eProcessMethod	= eProcessMethod;
	_mainStreamIndex = indexBasedStream;

	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		switch( _eProcessMethod )
		{
			case eProcessMethodShortest :
				if( _streamTranscoders.at( i )->getDuration() == getMinTotalDuration() )
					_streamTranscoders.at( i )->setInfinityStream( false );
				else
					_streamTranscoders.at( i )->setInfinityStream( true );
				break;
			case eProcessMethodLongest :
				if( _streamTranscoders.at( i )->getDuration() == getMaxTotalDuration() )
					_streamTranscoders.at( i )->setInfinityStream( false );
				else
					_streamTranscoders.at( i )->setInfinityStream( true );
				break;
			case eProcessMethodBasedOnStream :
				if( i != _mainStreamIndex )
					_streamTranscoders.at( i )->setInfinityStream( true );
				else
					_streamTranscoders.at( i )->setInfinityStream( false );
				break;
			case eProcessMethodInfinity :
				_streamTranscoders.at( i )->setInfinityStream( true );
				break;
		}
	}
}

void Transcoder::setVerbose( bool verbose )
{
	_verbose = verbose;
	for( std::vector< StreamTranscoder* >::iterator it = _streamTranscoders.begin(); it != _streamTranscoders.end(); ++it )
	{
		(*it)->setVerbose( _verbose );
	}
	_outputFile.setVerbose( _verbose );
}

void Transcoder::addRewrapStream( const std::string& filename, const size_t streamIndex )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	_streamTranscodersAllocated.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile ) );
	_streamTranscoders.push_back( _streamTranscodersAllocated.back() );
}

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const size_t subStreamIndex, const size_t offset )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	// Create profile as input configuration
	NoDisplayProgress progress;
	referenceFile->analyse( progress, InputFile::eAnalyseLevelFast );
	AudioProperties audioProperties = referenceFile->getProperties().audioStreams.at( streamIndex );

	ProfileLoader::Profile profile;
	profile[ constants::avProfileIdentificator ] = "presetRewrap";
	profile[ constants::avProfileIdentificatorHuman ] = "Preset rewrap";
	profile[ constants::avProfileType ] = avtranscoder::constants::avProfileTypeAudio;
	profile[ constants::avProfileCodec ] = audioProperties.codecName;
	profile[ constants::avProfileSampleFormat ] = audioProperties.sampleFormatName;
	std::stringstream ss;
	ss << audioProperties.sampleRate;
	profile[ constants::avProfileSampleRate ] = ss.str();
	profile[ constants::avProfileChannel ] = "1";

	// Add profile
	_profileLoader.update( profile );

	switch( referenceFile->getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		case AVMEDIA_TYPE_AUDIO:
		{
			_streamTranscodersAllocated.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile, subStreamIndex , offset ) );
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

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const size_t subStreamIndex, ProfileLoader::Profile& profile, const size_t offset )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	switch( referenceFile->getStreamType( streamIndex ) )
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
	if( ! profile.count( constants::avProfileType ) )
		throw std::runtime_error( "unable to found stream type (audio, video, etc.)" );

	if( profile.find( constants::avProfileType )->second == constants::avProfileTypeVideo )
	{
		if( _verbose )
			std::cout << "add a generated video stream" << std::endl;

		GeneratorVideo* video = new GeneratorVideo();
		video->setVideoFrameDesc( static_cast<const VideoCodec&>( codec ).getVideoFrameDesc() );
		_internalEssences.push_back( video );

		_streamTranscodersAllocated.push_back( new StreamTranscoder( *video, _outputFile, profile ) );
		_streamTranscoders.push_back( _streamTranscodersAllocated.back() );
	}
	else if( profile.find( constants::avProfileType )->second == constants::avProfileTypeAudio )
	{
		if( _verbose )
			std::cout << "add a generated audio stream" << std::endl;

		GeneratorAudio* audio = new GeneratorAudio();
		audio->setAudioFrameDesc( static_cast<const AudioCodec&>( codec ).getAudioFrameDesc() );
		_internalEssences.push_back( audio );

		_streamTranscodersAllocated.push_back( new StreamTranscoder( *audio, _outputFile, profile ) );
		_streamTranscoders.push_back( _streamTranscodersAllocated.back() );
	}
}

InputFile* Transcoder::addInputFile( const std::string& filename, const size_t streamIndex )
{
	InputFile* referenceFile = NULL;

	for( std::vector< InputFile* >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		if( ( (*it)->getFilename() == filename ) &&
			( ! (*it)->getReadStream( streamIndex ) ) )
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

	referenceFile->readStream( streamIndex );

	return referenceFile;
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

}
