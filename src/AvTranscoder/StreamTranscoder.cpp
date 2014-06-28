
#include "StreamTranscoder.hpp"

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder( InputStream& stream, OutputFile& outputFile, const size_t& streamId )
	: _stream( &stream )
	, _frameBuffer( NULL )
	, _inputEssence( NULL )
	, _outputEssence( NULL )
	, _outputFile( &outputFile )
	, _streamIndex( streamId )
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
	
	switch( _stream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_inputEssence = new InputVideo( *static_cast<AvInputStream*>( _stream ) );
			_inputEssence->setup();

			// re-wrap only, get output descriptor from input
			if( profile.empty() )
			{
				_outputFile->addVideoStream( _stream->getVideoDesc() );
				break;
			}

			OutputVideo* outputVideo = new OutputVideo();
			_outputEssence = outputVideo;

			_outputEssence->setProfile( profile );
			_outputFile->addVideoStream( outputVideo->getVideoDesc() );
			_videoFrameBuffer = new Image( outputVideo->getVideoDesc().getImageDesc() );
			_frameBuffer = _videoFrameBuffer;
			
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputEssence = new InputAudio( *static_cast<AvInputStream*>( _stream ) );
			_inputEssence->setup();

			// re-wrap only, get output descriptor from input
			if( profile.empty() )
			{
				_outputFile->addAudioStream( _stream->getAudioDesc() );
				break;
			}
			
			OutputAudio* outputAudio = new OutputAudio();
			_outputEssence = outputAudio;

			_outputEssence->setProfile( profile );
			_outputFile->addAudioStream( outputAudio->getAudioDesc() );
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
		if( ! _stream->readNextPacket( dataStream ) )
			return false;
		_outputFile->wrap( dataStream, _streamIndex );
		return true;
	}

	// std::cout << "encode & wrap" << _streamIndex << std::endl;

	if( _inputEssence->readNextFrame( *_frameBuffer ) )
	{
		_outputEssence->encodeFrame( *_frameBuffer, dataStream );
	}
	else if( ! _outputEssence->encodeFrame( dataStream ) )
	{
		return false;
	}

	_outputFile->wrap( dataStream, _streamIndex );
	return true;
}

}
