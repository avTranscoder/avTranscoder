#ifndef _AV_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _AV_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <AvTranscoder/CodedStream/InputStream.hpp>

#include <AvTranscoder/EssenceStream/InputVideo.hpp>
#include <AvTranscoder/EssenceStream/OutputVideo.hpp>

#include <AvTranscoder/EssenceStream/InputAudio.hpp>
#include <AvTranscoder/EssenceStream/OutputAudio.hpp>

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

	InputEssence*  _inputEssence;
	OutputEssence* _outputEssence;
	OutputFile*    _outputFile;

	size_t _streamIndex;

	bool _transcodeStream;


};
	
}

#endif

