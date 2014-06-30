
#include "StreamTranscoder.hpp"

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder( InputStream& inputStream, OutputStream& outputStream )
	: _inputStream( &inputStream )
	, _outputStream( &outputStream )
	, _frameBuffer( NULL )
	, _videoFrameBuffer( NULL )
	, _audioFrameBuffer( NULL )
	, _inputEssence( NULL )
	, _outputEssence( NULL )
	, _transcodeStream( false )
{

}

StreamTranscoder::StreamTranscoder( InputStream& inputStream, OutputStream& outputStream, Profile::ProfileDesc& profile )
	: _inputStream( &inputStream )
	, _outputStream( &outputStream )
	, _frameBuffer( NULL )
	, _videoFrameBuffer( NULL )
	, _audioFrameBuffer( NULL )
	, _inputEssence( NULL )
	, _outputEssence( NULL )
	, _transcodeStream( false )
{

}

StreamTranscoder::StreamTranscoder( InputEssence& inputEssence, OutputStream& outputStream, Profile::ProfileDesc& profile )
	: _inputStream( NULL )
	, _outputStream( &outputStream )
	, _frameBuffer( NULL )
	, _videoFrameBuffer( NULL )
	, _audioFrameBuffer( NULL )
	, _inputEssence( &inputEssence )
	, _outputEssence( NULL )
	, _transcodeStream( false )
{
}

StreamTranscoder::~StreamTranscoder()
{
	if( _frameBuffer )
		delete _frameBuffer;
	if( _inputEssence )
		delete _inputEssence;
	if( _outputEssence )
		delete _outputEssence;
}

void StreamTranscoder::init( const std::string& profile )
{
	_transcodeStream = profile.size();
	
	switch( _inputStream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_inputEssence = new InputVideo( *static_cast<AvInputStream*>( _inputStream ) );
			_inputEssence->setup();

			// re-wrap only, get output descriptor from input
			if( profile.empty() )
			{
				// _outputFile->addVideoStream( _inputStream->getVideoDesc() );
				break;
			}

			OutputVideo* outputVideo = new OutputVideo();
			_outputEssence = outputVideo;

			_outputEssence->setProfile( profile );
			// _outputFile->addVideoStream( outputVideo->getVideoDesc() );
			_videoFrameBuffer = new Image( outputVideo->getVideoDesc().getImageDesc() );
			_frameBuffer = _videoFrameBuffer;
			
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputEssence = new InputAudio( *static_cast<AvInputStream*>( _inputStream ) );
			_inputEssence->setup();

			// re-wrap only, get output descriptor from input
			if( profile.empty() )
			{
				// _outputFile->addAudioStream( _inputStream->getAudioDesc() );
				break;
			}
			
			OutputAudio* outputAudio = new OutputAudio();
			_outputEssence = outputAudio;

			_outputEssence->setProfile( profile );
			// _outputFile->addAudioStream( outputAudio->getAudioDesc() );
			_audioFrameBuffer = new AudioFrame( outputAudio->getAudioDesc().getFrameDesc() );
			_frameBuffer = _audioFrameBuffer;
			
			break;
		}
		default:
			break;
	}
}

bool StreamTranscoder::processFrame()
{
	DataStream dataStream;
	if( ! _transcodeStream )
	{
		if( ! _inputStream->readNextPacket( dataStream ) )
			return false;
		_outputStream->wrap( dataStream );
		return true;
	}

	if( _inputEssence->readNextFrame( *_frameBuffer ) )
	{
		_outputEssence->encodeFrame( *_frameBuffer, dataStream );
	}
	else if( ! _outputEssence->encodeFrame( dataStream ) )
	{
		return false;
	}

	_outputStream->wrap( dataStream );
	return true;
}

}
