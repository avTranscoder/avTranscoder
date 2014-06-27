
#include "StreamTranscoder.hpp"

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder( AvInputStream& stream, OutputFile& outputFile, const size_t& streamId )
	: _stream( &stream )
	, _frameBuffer( NULL )
	, _inputEssence( NULL )
	, _outputStreamWriter( NULL )
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
	if( _outputStreamWriter )
		delete _outputStreamWriter;
}

void StreamTranscoder::init( const std::string& profile )
{
	_transcodeStream = profile.size();
	
	switch( _stream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_inputEssence = new InputVideo( *_stream );
			_inputEssence->setup();

			// re-wrap only, get output descriptor from input
			if( profile.empty() )
			{
				_outputFile->addVideoStream( _stream->getVideoDesc() );
				break;
			}

			OutputStreamVideo* outputStreamVideo = new OutputStreamVideo();
			_outputStreamWriter = outputStreamVideo;

			_outputStreamWriter->setProfile( profile );
			_outputFile->addVideoStream( outputStreamVideo->getVideoDesc() );
			_videoFrameBuffer = new Image( outputStreamVideo->getVideoDesc().getImageDesc() );
			_frameBuffer = _videoFrameBuffer;
			
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputEssence = new InputAudio( *_stream );
			_inputEssence->setup();

			// re-wrap only, get output descriptor from input
			if( profile.empty() )
			{
				_outputFile->addAudioStream( _stream->getAudioDesc() );
				break;
			}
			
			OutputStreamAudio* outputStreamAudio = new OutputStreamAudio();
			_outputStreamWriter = outputStreamAudio;

			_outputStreamWriter->setProfile( profile );
			_outputFile->addAudioStream( outputStreamAudio->getAudioDesc() );
			_audioFrameBuffer = new AudioFrame( outputStreamAudio->getAudioDesc().getFrameDesc() );
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
		_outputStreamWriter->encodeFrame( *_frameBuffer, dataStream );
	}
	else if( ! _outputStreamWriter->encodeFrame( dataStream ) )
	{
		return false;
	}

	_outputFile->wrap( dataStream, _streamIndex );
	return true;
}

}
