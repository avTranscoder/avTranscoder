#ifndef _AV_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _AV_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <AvTranscoder/AvInputStream.hpp>
#include <AvTranscoder/InputStream.hpp>

#include <AvTranscoder/InputStreamVideo.hpp>
#include <AvTranscoder/OutputStreamVideo.hpp>

#include <AvTranscoder/InputStreamAudio.hpp>
#include <AvTranscoder/OutputStreamAudio.hpp>

namespace avtranscoder
{

class StreamTranscoder
{
public:
	StreamTranscoder( AvInputStream& stream, const bool isVideoStream );
	~StreamTranscoder();

	void init( const std::string& profile );

	bool processFrame();

private:
	AvInputStream* _stream;

	InputStreamVideo*  _inputStreamVideo;
	OutputStreamVideo* _outputStreamVideo;

	InputStreamAudio*  _inputStreamAudio;
	OutputStreamAudio* _outputStreamAudio;

	const bool _isVideoStream;

	bool _transcodeStream;

};
	
}

#endif
