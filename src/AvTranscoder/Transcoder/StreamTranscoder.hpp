#ifndef _AV_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _AV_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <AvTranscoder/CodedStream/InputStream.hpp>

#include <AvTranscoder/EssenceStream/InputVideo.hpp>
#include <AvTranscoder/EssenceStream/OutputVideo.hpp>

#include <AvTranscoder/EssenceStream/InputAudio.hpp>
#include <AvTranscoder/EssenceStream/OutputAudio.hpp>

#include <AvTranscoder/File/OutputFile.hpp>

#include <AvTranscoder/Profile.hpp>

namespace avtranscoder
{

class StreamTranscoder
{
public:
	/**
	 * @brief rewrap stream
	 **/
	StreamTranscoder( InputStream& inputStream, OutputStream& outputStream );

	/**
	 * @brief transcode stream
	 **/
	StreamTranscoder( InputStream& inputStream, OutputStream& outputStream, Profile::ProfileDesc& profile );

	/**
	 * @brief encode from dummy stream
	 **/
	StreamTranscoder( InputEssence& inputEssence, OutputStream& outputStream, Profile::ProfileDesc& profile );

	~StreamTranscoder();

	void init( const std::string& profile );

	bool processFrame();

	bool isTranscodeStream() const { return _transcodeStream; }

private:
	InputStream*   _inputStream;
	OutputStream*  _outputStream;

	Frame*         _frameBuffer;
	Image*         _videoFrameBuffer;
	AudioFrame*    _audioFrameBuffer;

	InputEssence*  _inputEssence;
	OutputEssence* _outputEssence;

	bool _transcodeStream;


};
	
}

#endif
