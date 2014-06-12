
#include "StreamTranscoder.hpp"


/*
InputFile : fichier

InputStream : déwrapper virtuel
AvInputStream : déwrapper AV
DummyInputStream : generate silence

InputStreamAudio : décoder audio
InputStreamVideo : décoder video

OutputStreamAudio : encoder audio
OutputStreamVideo : encoder video

OutputStream : encoder vide (futur: wrapper)
OutputFile : fichier + wrapper virtuel

*/

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder( AvInputStream& stream, const bool isVideoStream )
	: _stream( &stream )
	, _inputStreamVideo( NULL )
	, _outputStreamVideo( NULL )
	, _inputStreamAudio( NULL )
	, _outputStreamAudio( NULL )
	, _isVideoStream( isVideoStream )
	, _transcodeStream( false )
{
	// std::cout << "[StreamTranscoder::StreamTranscoder]" << std::endl;
}

StreamTranscoder::~StreamTranscoder()
{
	if( _inputStreamVideo )
		delete _inputStreamVideo;
	if( _outputStreamVideo )
		delete _outputStreamVideo;

	if( _inputStreamAudio )
		delete _inputStreamAudio;
	if( _outputStreamAudio )
		delete _outputStreamAudio;
}

void StreamTranscoder::init( const std::string& profile )
{
	std::cout << "[StreamTranscoder::init] isVideoStream: " << _isVideoStream << std::endl;
	std::cout << "[StreamTranscoder::init] profile: " << profile << std::endl;

	if( ! profile.empty() )
	{
		_transcodeStream = true;
		if( _isVideoStream )
		{
			std::cout << "[StreamTranscoder::init] video stream..." << std::endl;
			_inputStreamVideo = new InputStreamVideo( *_stream );
			_outputStreamVideo = new OutputStreamVideo();
			_outputStreamVideo->setProfile( profile );
		}
		else
		{
			std::cout << "[StreamTranscoder::init] audio stream..." << std::endl;
			_inputStreamAudio = new InputStreamAudio( *_stream );
			_outputStreamAudio = new OutputStreamAudio();
			_outputStreamAudio->setProfile( profile );
		}
	}
}

bool StreamTranscoder::processFrame()
{
	if( _transcodeStream )
	{
		// transcodes

	}
	// wraps

	return true;
}

}
