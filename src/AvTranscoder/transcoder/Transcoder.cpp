#include "Transcoder.hpp"

#include <limits>
#include <iostream>
#include <algorithm>

namespace avtranscoder
{

Transcoder::Transcoder( OutputFile& outputFile )
	: _outputFile( outputFile )
	, _inputFiles()
	, _inputStreams()
	, _streamTranscoders()
	, _generatorAudio()
	, _generatorVideo()
	, _profile( true )
	, _outputFps( 25 )
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
	for( std::vector< StreamTranscoder* >::iterator it = _streamTranscoders.begin(); it != _streamTranscoders.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< GeneratorAudio* >::iterator it = _generatorAudio.begin(); it != _generatorAudio.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< GeneratorVideo* >::iterator it = _generatorVideo.begin(); it != _generatorVideo.end(); ++it )
	{
		delete (*it);
	}
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName, const size_t offset )
{
	if( profileName.length() == 0 ) // no profile, only re-wrap stream
	{
		if( _verbose )
			std::cout << "add re-wrap stream" << std::endl;
		addRewrapStream( filename, streamIndex );
		return;
	}

	Profile::ProfileDesc& transcodeProfile = _profile.getProfile( profileName );
	add( filename, streamIndex, transcodeProfile, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const std::string& profileName, ICodec& codec, const size_t offset )
{
	if( profileName.length() == 0 ) // no profile, only re-wrap stream
	{
		if( _verbose )
			std::cout << "add re-wrap stream" << std::endl;
		
		if( filename.length() == 0 )
		{
			std::cerr << "can't add a generated stream with no profileName indicated" << std::endl;
			return;
		}
		
		addRewrapStream( filename, streamIndex );
		return;
	}
	
	Profile::ProfileDesc& transcodeProfile = _profile.getProfile( profileName );
	add( filename, streamIndex, transcodeProfile, codec, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profileDesc, const size_t offset )
{
	_profile.update( profileDesc );
	if( ! filename.length() )
	{
		std::cerr << "can't add a stream with no filename indicated" << std::endl;
		return;
	}

	if( _verbose )
		std::cout << "add transcoding stream" << std::endl;
	addTranscodeStream( filename, streamIndex, profileDesc, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profileDesc, ICodec& codec, const size_t offset )
{
	_profile.update( profileDesc );
	if( ! filename.length() )
	{
		if( _verbose )
			std::cout << "add a generated stream" << std::endl;
		addDummyStream( profileDesc, codec );
		return;
	}
	
	if( _verbose )
		std::cout << "add transcoding stream" << std::endl;
	addTranscodeStream( filename, streamIndex, profileDesc, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, const size_t offset )
{
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileName, offset );
		return;
	}

	if( profileName.length() == 0 ) // no profile, only re-wrap stream
	{
		if( _verbose )
			std::cout << "add re-wrap stream for substream " << subStreamIndex << std::endl;

		addRewrapStream( filename, streamIndex );
		return;
	}

	Profile::ProfileDesc& transcodeProfile = _profile.getProfile( profileName );
	add( filename, streamIndex, subStreamIndex, transcodeProfile, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, ICodec& codec, const size_t offset )
{
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileName, codec );
		return;
	}
	
	if( profileName.length() == 0 ) // no profile, only re-wrap stream
	{
		if( _verbose )
			std::cout << "add re-wrap stream for substream " << subStreamIndex << std::endl;

		if( filename.length() == 0 )
		{
			std::cerr << "can't add a generated stream with no profileName indicated" << std::endl;
			return;
		}
		
		addRewrapStream( filename, streamIndex );
		return;
	}

	Profile::ProfileDesc& transcodeProfile = _profile.getProfile( profileName );
	add( filename, streamIndex, subStreamIndex, transcodeProfile, codec, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, Profile::ProfileDesc& profileDesc, const size_t offset )
{
	_profile.update( profileDesc );
	
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileDesc, offset );
		return;
	}
	
	if( ! filename.length() )
	{
		if( _verbose )
			std::cerr << "can't add a stream with no filename indicated" << std::endl;
		return;
	}

	if( _verbose )
		std::cout << "add transcoding stream for substream " << subStreamIndex << std::endl;
	addTranscodeStream( filename, streamIndex, subStreamIndex, profileDesc, offset );
}

void Transcoder::add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, Profile::ProfileDesc& profileDesc, ICodec& codec, const size_t offset )
{
	_profile.update( profileDesc );
	
	if( subStreamIndex < 0 )
	{
		add( filename, streamIndex, profileDesc );
		return;
	}
	
	if( ! filename.length() )
	{
		if( _verbose )
			std::cout << "add a generated stream" << std::endl;
		addDummyStream( profileDesc, codec );
		return;
	}

	if( _verbose )
		std::cout << "add transcoding stream for substream " << subStreamIndex << std::endl;
	addTranscodeStream( filename, streamIndex, subStreamIndex, profileDesc, offset );
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

	std::vector< CodedData > codedData;

	codedData.reserve( _inputStreams.size() );

	for( size_t streamIndex = 0; streamIndex < _inputStreams.size(); ++streamIndex )
	{
		CodedData data;
		codedData.push_back( data );
	}

	if( ! _inputStreams.size() &&
		! _generatorVideo.size() && 
		! _generatorAudio.size() )
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
		
		if( progress.progress( 1 / _outputFps * ( frame ), totalDuration ) == eJobStatusCancel )
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
	_streamTranscoders.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile ) );
	_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
}

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profile, const size_t offset )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	switch( referenceFile->getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		case AVMEDIA_TYPE_AUDIO:
		{
			_streamTranscoders.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile, -1 , offset ) );
			_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
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

void Transcoder::addTranscodeStream( const std::string& filename, const size_t streamIndex, const size_t subStreamIndex, Profile::ProfileDesc& profile, const size_t offset )
{
	InputFile* referenceFile = addInputFile( filename, streamIndex );

	switch( referenceFile->getStreamType( streamIndex ) )
	{
		case AVMEDIA_TYPE_VIDEO:
		case AVMEDIA_TYPE_AUDIO:
		{
			_streamTranscoders.push_back( new StreamTranscoder( referenceFile->getStream( streamIndex ), _outputFile, profile, subStreamIndex, offset ) );
			_inputStreams.push_back( &referenceFile->getStream( streamIndex ) );
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

void Transcoder::addDummyStream( const Profile::ProfileDesc& profile, const ICodec& codec )
{
	if( ! profile.count( Profile::avProfileType ) )
		throw std::runtime_error( "unable to found stream type (audio, video, etc.)" );

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeAudio )
	{
		if( _verbose )
			std::cout << "add a generated audio stream" << std::endl;
		_generatorAudio.push_back( new GeneratorAudio() );
		_generatorAudio.back()->setAudioDesc( static_cast<AudioCodec>( codec ) );
		
		_streamTranscoders.push_back( new StreamTranscoder( *_generatorAudio.back(), _outputFile, profile ) );
	}

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeVideo )
	{
		if( _verbose )
			std::cout << "add generated video stream" << std::endl;
		_generatorVideo.push_back( new GeneratorVideo() );
		_generatorVideo.back()->setVideoDesc( static_cast<VideoCodec>( codec ) );
		
		_streamTranscoders.push_back( new StreamTranscoder( *_generatorVideo.back(), _outputFile, profile ) );
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
