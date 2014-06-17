
#include "StreamTranscoder.hpp"

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder( AvInputStream& stream, OutputFile& outputFile, const size_t& streamId )
	: _stream( &stream )
	, _frameBuffer( NULL )
	, _inputStreamReader( NULL )
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
	if( _inputStreamReader )
		delete _inputStreamReader;
	if( _outputStreamWriter )
		delete _outputStreamWriter;
}

void StreamTranscoder::init( const std::string& profile )
{
	if( profile.empty() )
	{
		return;
	}

	_transcodeStream = true;

	switch( _stream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_inputStreamReader = new InputStreamVideo( *_stream );
			_outputStreamWriter = new OutputStreamVideo();
			_outputStreamWriter->setProfile( profile );
			_frameBuffer = new Image( _stream->getVideoDesc().getImageDesc() );
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputStreamReader = new InputStreamAudio( *_stream );
			_outputStreamWriter = new OutputStreamAudio();
			_outputStreamWriter->setProfile( profile );
			_frameBuffer = new AudioFrame( _stream->getAudioDesc().getFrameDesc() );
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
		_stream->readNextPacket( dataStream );
		_outputFile->wrap( dataStream, _streamIndex );
		return true;
	}
	// transcodes
	_inputStreamReader->readNextFrame( *_frameBuffer );
	_outputStreamWriter->encodeFrame( *_frameBuffer, dataStream );

	_outputFile->wrap( dataStream, _streamIndex );
	return true;
}

}
