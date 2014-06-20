
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

			if( ! profile.empty() )
			{
				_outputStreamWriter = new OutputStreamVideo();
				OutputStreamVideo* outputStreamVideo = static_cast<OutputStreamVideo*>( _outputStreamWriter );

				outputStreamVideo->setProfile( profile );
				_outputFile->addVideoStream( outputStreamVideo->getVideoDesc() );
				_frameBuffer = new Image( outputStreamVideo->getVideoDesc().getImageDesc() );
			}
			else
			{
				_outputFile->addVideoStream( _stream->getVideoDesc() );
			}

			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputStreamReader = new InputStreamAudio( *_stream );
			_inputStreamReader->setup();
			if( ! profile.empty() )
			{
				_outputStreamWriter = new OutputStreamAudio();
				OutputStreamAudio* outputStreamAudio = static_cast<OutputStreamAudio*>( _outputStreamWriter );

				outputStreamAudio->setProfile( profile );
				_outputFile->addAudioStream( outputStreamAudio->getAudioDesc() );
				_frameBuffer = new AudioFrame( outputStreamAudio->getAudioDesc().getFrameDesc() );
			}
			else
			{
				_outputFile->addAudioStream( _stream->getAudioDesc() );
			}

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
