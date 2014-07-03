
#include "StreamTranscoder.hpp"

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

#include <AvTranscoder/EssenceTransform/AudioEssenceTransform.hpp>
#include <AvTranscoder/EssenceTransform/VideoEssenceTransform.hpp>

#include <cassert>

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder(
		InputStream& inputStream,
		OutputFile& outputFile
	)
	: _inputStream( &inputStream )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _videoFrameBuffer( NULL )
	, _audioFrameBuffer( NULL )
	, _inputEssence( NULL )
	, _outputEssence( NULL )
	, _transform( NULL )
	, _transcodeStream( false )
{
	// create a re-wrapping case
	switch( _inputStream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_outputStream = &outputFile.addVideoStream( _inputStream->getVideoDesc() );
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_outputStream = &outputFile.addAudioStream( _inputStream->getAudioDesc() );
			break;
		}
		default:
			break;
	}
}

StreamTranscoder::StreamTranscoder(
		InputStream& inputStream,
		OutputFile& outputFile,
		Profile::ProfileDesc& profile
	)
	: _inputStream( &inputStream )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _videoFrameBuffer( NULL )
	, _audioFrameBuffer( NULL )
	, _inputEssence( NULL )
	, _outputEssence( NULL )
	, _transform( NULL )
	, _transcodeStream( true )
{
	// create a transcode case
	switch( _inputStream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_inputEssence = new InputVideo( *static_cast<AvInputStream*>( _inputStream ) );
			_inputEssence->setup();

			OutputVideo* outputVideo = new OutputVideo();

			_outputEssence = outputVideo;
			_outputEssence->setProfile( profile );
			
			_outputStream = &outputFile.addVideoStream( outputVideo->getVideoDesc() );

			_sourceBuffer = new Image( _inputStream->getVideoDesc().getImageDesc() );
			_videoFrameBuffer = new Image( outputVideo->getVideoDesc().getImageDesc() );
			_frameBuffer = _videoFrameBuffer;
			
			_transform = new VideoEssenceTransform();

			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputEssence = new InputAudio( *static_cast<AvInputStream*>( _inputStream ) );
			_inputEssence->setup();
			
			OutputAudio* outputAudio = new OutputAudio();

			_outputEssence = outputAudio;
			_outputEssence->setProfile( profile );

			_outputStream = &outputFile.addAudioStream( outputAudio->getAudioDesc() );

			_sourceBuffer = new AudioFrame( _inputStream->getAudioDesc().getFrameDesc() );
			_audioFrameBuffer = new AudioFrame( outputAudio->getAudioDesc().getFrameDesc() );
			_frameBuffer = _audioFrameBuffer;
			
			_transform = new AudioEssenceTransform();

			break;
		}
		default:
		{
			throw std::runtime_error( "unupported stream type" );
			break;
		}
	}
}

StreamTranscoder::StreamTranscoder(
		InputEssence& inputEssence,
		OutputFile& outputFile,
		Profile::ProfileDesc& profile
	)
	: _inputStream( NULL )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _videoFrameBuffer( NULL )
	, _audioFrameBuffer( NULL )
	, _inputEssence( &inputEssence )
	, _outputEssence( NULL )
	, _transform( NULL )
	, _transcodeStream( true )
{
	if( ! profile.count( Profile::avProfileType ) )
		throw std::runtime_error( "unable to found stream type (audio, video, etc.)" );

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeAudio )
	{
		OutputAudio* outputAudio = new OutputAudio();

		_outputEssence = outputAudio;
		_outputEssence->setProfile( profile );

		_outputStream = &outputFile.addAudioStream( outputAudio->getAudioDesc() );
		_audioFrameBuffer = new AudioFrame( outputAudio->getAudioDesc().getFrameDesc() );
		_frameBuffer = _audioFrameBuffer;
		return;
	}

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeVideo )
	{
		OutputVideo* outputVideo = new OutputVideo();

		_outputEssence = outputVideo;
		_outputEssence->setProfile( profile );

		_outputStream = &outputFile.addVideoStream( outputVideo->getVideoDesc() );
		_videoFrameBuffer = new Image( outputVideo->getVideoDesc().getImageDesc() );
		_frameBuffer = _videoFrameBuffer;
		return;
	}

	throw std::runtime_error( "unupported stream type" );
}

StreamTranscoder::~StreamTranscoder()
{
	if( _frameBuffer )
		delete _frameBuffer;
	if( _inputEssence )
		delete _inputEssence;
	if( _outputEssence )
		delete _outputEssence;
	if( _transform )
		delete _transform;
}


bool StreamTranscoder::processFrame()
{
	if( _transcodeStream )
	{
		return processTranscode();
	}
	return processRewrap();
}

bool StreamTranscoder::processRewrap()
{
	assert( _inputStream  != NULL );
	
	DataStream dataStream;
	if( ! _inputStream->readNextPacket( dataStream ) )
		return false;
	_outputStream->wrap( dataStream );
	return true;
}

bool StreamTranscoder::processTranscode()
{
	assert( _inputEssence  != NULL );
	assert( _outputEssence != NULL );
	assert( _sourceBuffer  != NULL );
	assert( _frameBuffer   != NULL );

	std::cout << "transcode" << std::endl; 

	DataStream dataStream;
	if( _inputEssence->readNextFrame( *_sourceBuffer ) )
	{
		_transform->convert( *_sourceBuffer, *_frameBuffer );
		_outputEssence->encodeFrame( *_frameBuffer, dataStream );
	}
	else
	{
		std::cout << "encode last frame" << std::endl;
		if( ! _outputEssence->encodeFrame( dataStream ) )
		{
			return false;
		}
	}

	std::cout << "wrap" << std::endl;
	_outputStream->wrap( dataStream );
	return true;
}

}
