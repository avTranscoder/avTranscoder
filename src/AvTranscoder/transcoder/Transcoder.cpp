#include "Transcoder.hpp"

#include <AvTranscoder/file/util.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/stat/VideoStat.hpp>

#include <limits>
#include <algorithm>

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

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName, const float offset )
{
	// Re-wrap
	if( profileName.length() == 0 )
	{
		// Check filename
		if( filename.length() == 0 )
			throw std::runtime_error( "Can't re-wrap a stream without filename indicated" );

		addRewrapStream( filename, streamIndex, offset );
	}
	// Transcode
	else
	{
		const ProfileLoader::Profile& transcodeProfile = _profileLoader.getProfile( profileName );
		add( filename, streamIndex, transcodeProfile, offset );
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName, ICodec& codec, const float offset )
{
	// Re-wrap
	if( profileName.length() == 0 )
	{
		// Check filename
		if( filename.length() == 0 )
			throw std::runtime_error( "Can't re-wrap a stream without filename indicated" );
		
		addRewrapStream( filename, streamIndex, offset );
	}
	// Transcode
	else
	{
		const ProfileLoader::Profile& transcodeProfile = _profileLoader.getProfile( profileName );
		add( filename, streamIndex, transcodeProfile, codec, offset );
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const ProfileLoader::Profile& profile, const float offset )
{
	// Check filename
	if( ! filename.length() )
		throw std::runtime_error( "Can't transcode a stream without filename indicated" );

	addTranscodeStream( filename, streamIndex, -1, profile, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const ProfileLoader::Profile& profile, ICodec& codec, const float offset )
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

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, const float offset )
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
			addRewrapStream( filename, streamIndex, offset );
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
		const ProfileLoader::Profile& transcodeProfile = _profileLoader.getProfile( profileName );
		add( filename, streamIndex, subStreamIndex, transcodeProfile, offset );
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, ICodec& codec, const float offset )
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
			addRewrapStream( filename, streamIndex, offset );
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
		const ProfileLoader::Profile& transcodeProfile = _profileLoader.getProfile( profileName );
		add( filename, streamIndex, subStreamIndex, transcodeProfile, codec, offset );
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const ProfileLoader::Profile& profile, const float offset )
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

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const ProfileLoader::Profile& profile, ICodec& codec, const float offset )
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
		LOG_DEBUG( "Init stream " << streamIndex )
		_streamTranscoders.at( streamIndex )->preProcessCodecLatency();
	}
}

bool Transcoder::processFrame()
{
	if( _streamTranscoders.size() == 0 )
		return false;

	for( size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex )
	{
		LOG_DEBUG( "Process stream " << streamIndex << "/" << ( _streamTranscoders.size() - 1 ) )

		bool streamProcessStatus = _streamTranscoders.at( streamIndex )->processFrame();
		if( ! streamProcessStatus )
		{
			return false;
		}
	}
	return true;
}

ProcessStat Transcoder::process()
{
	NoDisplayProgress progress;
	return process( progress );
}

ProcessStat Transcoder::process( IProgress& progress )
{
	if( _streamTranscoders.size() == 0 )
		throw std::runtime_error( "Missing input streams in transcoder" );

	manageSwitchToGenerator();

	LOG_INFO( "Start process" )

	_outputFile.beginWrap();

	preProcessCodecLatency();

	const double outputDuration = getOutputDuration();
	LOG_INFO( "Output duration of the process will be " << outputDuration << "s." )

	size_t frame = 0;
	bool frameProcessed = true;
	while( frameProcessed )
	{
		const double progressDuration = _outputFile.getStream( 0 ).getStreamDuration();

		// check if JobStatusCancel
		if( progress.progress( ( progressDuration > outputDuration ) ? outputDuration : progressDuration, outputDuration ) == eJobStatusCancel )
			break;

		// check progressDuration
		if( progressDuration >= outputDuration )
			break;

		LOG_DEBUG( "Process frame " << frame )
		frameProcessed =  processFrame();

		++frame;
	}

	_outputFile.endWrap();

	LOG_INFO( "End of process" )

	LOG_INFO( "Get process statistics" )
	ProcessStat processStat;
	fillProcessStat( processStat );

	return processStat;
}

void Transcoder::setProcessMethod( const EProcessMethod eProcessMethod, const size_t indexBasedStream, const double outputDuration )
{
	_eProcessMethod	= eProcessMethod;
	_mainStreamIndex = indexBasedStream;
	_outputDuration = outputDuration;
}

void Transcoder::addRewrapStream( const std::string& filename, const size_t streamIndex, const float offset )
{
	LOG_INFO( "Add rewrap stream from file '" << filename << "' / index=" << streamIndex << " / offset=" << offset << "s"  )

	InputFile* referenceFile = addInputFile( filename, streamIndex, offset );

	_streamTranscodersAllocated.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, offset ) );
	_streamTranscoders.push_back( _streamTranscodersAllocated.back() );
}

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const float offset )
{
	// Get profile from input file
	InputFile inputFile( filename );
	ProfileLoader::Profile profile = getProfileFromFile( inputFile, streamIndex );

	// override channels parameter to manage demultiplexing
	ProfileLoader::Profile::iterator it = profile.find( constants::avProfileChannel );
	if( it != profile.end() )
		it->second = "1";

	addTranscodeStream( filename, streamIndex, subStreamIndex, profile, offset );
}

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const ProfileLoader::Profile& profile, const float offset )
{
	// Add profile
	if( ! _profileLoader.hasProfile( profile ) )
		_profileLoader.loadProfile( profile );

	LOG_INFO( "Add transcode stream from file '" << filename << "' / index=" << streamIndex << " / channel=" << subStreamIndex << " / encodingProfile=" << profile.at( constants::avProfileIdentificatorHuman ) << " / offset=" << offset << "s" )

	// Add input file
	InputFile* referenceFile = addInputFile( filename, streamIndex, offset );

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
	if( ! _profileLoader.hasProfile( profile ) )
		_profileLoader.loadProfile( profile );

	LOG_INFO( "Add generated stream with codec '" << codec.getCodecName() << "' / encodingProfile=" << profile.at( constants::avProfileIdentificatorHuman ) )

	_streamTranscodersAllocated.push_back( new StreamTranscoder( codec, _outputFile, profile ) );
	_streamTranscoders.push_back( _streamTranscodersAllocated.back() );
}

InputFile* Transcoder::addInputFile( const std::string& filename, const size_t streamIndex, const float offset )
{
	InputFile* referenceFile = NULL;

	for( std::vector< InputFile* >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		if( ( (*it)->getFilename() == filename ) &&
			! (*it)->getStream( streamIndex ).isActivated() )
		{
			referenceFile = (*it);
			LOG_DEBUG( "Get instance of InputFile from '" << filename << "'" )
			break;
		}
	}

	if( ! referenceFile )
	{
		LOG_DEBUG( "New instance of InputFile from '" << filename << "'" )

		_inputFiles.push_back( new InputFile( filename ) );
		referenceFile = _inputFiles.back();
	}

	referenceFile->activateStream( streamIndex );

	// If negative offset, move forward in the input stream
	if( offset < 0 )
		referenceFile->seekAtTime( -offset );

	return referenceFile;
}

ProfileLoader::Profile Transcoder::getProfileFromFile( InputFile& inputFile, const size_t streamIndex )
{
	const StreamProperties* streamProperties = &inputFile.getProperties().getStreamPropertiesWithIndex( streamIndex );
	const VideoProperties* videoProperties = NULL;
	const AudioProperties* audioProperties = NULL;
	switch( inputFile.getStream( streamIndex ).getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO:
		{
			videoProperties = dynamic_cast<const VideoProperties*>( streamProperties );
			break;
		}
		case AVMEDIA_TYPE_AUDIO:
		{
			audioProperties = dynamic_cast<const AudioProperties*>( streamProperties );
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

float Transcoder::getStreamDuration( size_t indexStream ) const
{
	return _streamTranscoders.at( indexStream )->getDuration();
}

float Transcoder::getMinTotalDuration() const
{
	float minTotalDuration = std::numeric_limits<float>::max();
	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		minTotalDuration = std::min( getStreamDuration( i ), minTotalDuration );
	}
	return minTotalDuration;
}

float Transcoder::getMaxTotalDuration() const
{
	float maxTotalDuration = 0;
	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		maxTotalDuration = std::max( getStreamDuration( i ), maxTotalDuration );
	}
	return maxTotalDuration;
}

float Transcoder::getOutputDuration() const
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
			return std::numeric_limits<float>::max();
		default:
			return getMaxTotalDuration();
	}	
}

void Transcoder::manageSwitchToGenerator()
{
	for( size_t i = 0; i < _streamTranscoders.size(); ++i )
	{
		const float currentDuration = _streamTranscoders.at( i )->getDuration();
		switch( _eProcessMethod )
		{
			case eProcessMethodShortest :
				if( _streamTranscoders.at( i )->getDuration() >= getMinTotalDuration() )
					_streamTranscoders.at( i )->needToSwitchToGenerator( false );
				else
					_streamTranscoders.at( i )->needToSwitchToGenerator();
				break;
			case eProcessMethodLongest :
				if( _streamTranscoders.at( i )->getDuration() == getMaxTotalDuration() )
					_streamTranscoders.at( i )->needToSwitchToGenerator( false );
				else
					_streamTranscoders.at( i )->needToSwitchToGenerator();
				break;
			case eProcessMethodBasedOnStream :
				if( i != _mainStreamIndex && currentDuration < _streamTranscoders.at( _mainStreamIndex )->getDuration() )
					_streamTranscoders.at( i )->needToSwitchToGenerator();
				else
					_streamTranscoders.at( i )->needToSwitchToGenerator( false );
				break;
			case eProcessMethodBasedOnDuration :
				if( _streamTranscoders.at( i )->getDuration() >= _outputDuration )
					_streamTranscoders.at( i )->needToSwitchToGenerator( false );
				else
					_streamTranscoders.at( i )->needToSwitchToGenerator();
				break;
			case eProcessMethodInfinity :
				_streamTranscoders.at( i )->needToSwitchToGenerator();
				break;
		}
	}
}

void Transcoder::fillProcessStat( ProcessStat& processStat )
{
	for( size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex )
	{
		IOutputStream& stream = _streamTranscoders.at( streamIndex )->getOutputStream();
		const AVMediaType mediaType = _streamTranscoders.at( streamIndex )->getInputStream().getStreamType();
		switch( mediaType )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				VideoStat videoStat( stream.getStreamDuration(), stream.getNbFrames() );
				const AVCodecContext& encoderContext = _streamTranscoders.at( streamIndex )->getEncoder().getCodec().getAVCodecContext();
				if( encoderContext.coded_frame && ( encoderContext.flags & CODEC_FLAG_PSNR) )
				{
					videoStat._quality = encoderContext.coded_frame->quality;
					videoStat._psnr = VideoStat::psnr( encoderContext.coded_frame->error[0] / ( encoderContext.width * encoderContext.height * 255.0 * 255.0 ) );
				}
				processStat.addVideoStat( streamIndex, videoStat );
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				AudioStat audioStat( stream.getStreamDuration(), stream.getNbFrames() );
				processStat.addAudioStat( streamIndex, audioStat );
				break;
			}
			default:
				LOG_WARN( "No process statistics for stream at index: " << streamIndex << " (AVMediaType = " << mediaType << ")" )
				break;
		}
	}
}

}
