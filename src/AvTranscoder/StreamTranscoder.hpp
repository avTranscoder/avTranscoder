#ifndef _AV_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _AV_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <AvTranscoder/AvInputStream.hpp>
#include <AvTranscoder/InputStream.hpp>

#include <AvTranscoder/InputStreamVideo.hpp>
#include <AvTranscoder/OutputStreamVideo.hpp>

#include <AvTranscoder/InputStreamAudio.hpp>
#include <AvTranscoder/OutputStreamAudio.hpp>

#include <AvTranscoder/File/OutputFile.hpp>


namespace avtranscoder
{

class StreamTranscoder
{
public:
	StreamTranscoder( InputStream& stream, OutputFile& outputFile, const size_t& streamId );
	~StreamTranscoder();

	void init( const std::string& profile );

	bool processFrame();

	bool isTranscodeStream() const { return _transcodeStream; }

private:
	InputStream* _stream;

	Frame*      _frameBuffer;
	Image*      _videoFrameBuffer;
	AudioFrame* _audioFrameBuffer;

	InputStreamReader*  _inputStreamReader;
	OutputStreamWriter* _outputStreamWriter;
	OutputFile* _outputFile;

	size_t _streamIndex;

	bool _transcodeStream;


};
	
}

#endif

