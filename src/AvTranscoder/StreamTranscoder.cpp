
#include "StreamTranscoder.hpp"

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder( AvInputStream& stream, OutputFile& outputFile, const size_t& streamId )
	: _stream( &stream )
	, _frameBuffer( NULL )
	, _inputStreamReader( NULL )
	, _inputStreamVideo( NULL )
	, _inputStreamAudio( NULL )
	, _outputStreamWriter( NULL )
	, _outputStreamVideo( NULL )
	, _outputStreamAudio( NULL )
	, _outputFile( &outputFile )
	, _streamIndex( streamId )
	, _transcodeStream( false )
{
}

StreamTranscoder::~StreamTranscoder()
{
	if( _frameBuffer )
		delete _frameBuffer;

	if( _inputStreamVideo )
		delete _inputStreamVideo;
	if( _inputStreamAudio )
		delete _inputStreamAudio;

	if( _outputStreamVideo )
		delete _outputStreamVideo;
	if( _outputStreamAudio )
		delete _outputStreamAudio;
}

void StreamTranscoder::init( const std::string& profile )
{
	_transcodeStream = profile.size();
	
	switch( _stream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_inputStreamVideo = new InputStreamVideo( *_stream );
			_inputStreamReader = _inputStreamVideo;
			_inputStreamReader->setup();

			// re-wrap only, get output descriptor from input
			if( profile.empty() )
			{
				_outputFile->addVideoStream( _stream->getVideoDesc() );
				break;
			}

			_outputStreamVideo = new OutputStreamVideo();
			_outputStreamWriter = _outputStreamVideo;

			_outputStreamWriter->setProfile( profile );
			_outputFile->addVideoStream( _outputStreamVideo->getVideoDesc() );
			_videoFrameBuffer = new Image( _outputStreamVideo->getVideoDesc().getImageDesc() );
			_frameBuffer = _videoFrameBuffer;
			
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputStreamAudio = new InputStreamAudio( *_stream );
			_inputStreamReader = _inputStreamAudio;
			_inputStreamReader->setup();

			// re-wrap only, get output descriptor from input
			if( profile.empty() )
			{
				_outputFile->addAudioStream( _stream->getAudioDesc() );
				break;
			}
			
			_outputStreamAudio = new OutputStreamAudio();
			_outputStreamWriter = _outputStreamAudio;

			_outputStreamWriter->setProfile( profile );
			_outputFile->addAudioStream( _outputStreamAudio->getAudioDesc() );
			_audioFrameBuffer = new AudioFrame( _outputStreamAudio->getAudioDesc().getFrameDesc() );
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

	if( _inputStreamReader->readNextFrame( *_frameBuffer ) )
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
