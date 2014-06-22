
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
	_transcodeStream = profile.size();
	
	switch( _stream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_inputStreamReader = new InputStreamVideo( *_stream );
			_inputStreamReader->setup();

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
			_frameBuffer = new Image( outputStreamVideo->getVideoDesc().getImageDesc() );
			
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputStreamReader = new InputStreamAudio( *_stream );
			_inputStreamReader->setup();

			// re-wrap only, get output descriptor from input
			if( profile.empty() )
			{
				_outputFile->addAudioStream( _stream->getAudioDesc() );
				break;
			}
			
			OutputStreamAudio* outputStreamAudio = new OutputStreamAudio();
			_outputStreamWriter = outputStreamAudio;

			outputStreamAudio->setProfile( profile );
			_outputFile->addAudioStream( outputStreamAudio->getAudioDesc() );
			_frameBuffer = new AudioFrame( outputStreamAudio->getAudioDesc().getFrameDesc() );
			
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
