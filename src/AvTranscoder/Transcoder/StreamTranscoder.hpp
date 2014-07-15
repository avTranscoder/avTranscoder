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

class EssenceTransform;

class StreamTranscoder
{
public:
	/**
	 * @brief rewrap stream
	 **/
	StreamTranscoder( InputStream& inputStream, OutputFile& outputFile );

	/**
	 * @brief transcode stream
	 **/
	StreamTranscoder( InputStream& inputStream, OutputFile& outputFile, Profile::ProfileDesc& profile );

	/**
	 * @brief encode from dummy stream
	 **/
	StreamTranscoder( InputEssence& inputEssence, OutputFile& outputFile, Profile::ProfileDesc& profile );

	~StreamTranscoder();

	/**
	 * @brief process a single frame for the current stream
	 * @return the process status result
	 */
	bool processFrame();

	bool isTranscodeStream() const { return _transcodeStream; }

private:
	bool processRewrap();
	bool processTranscode();

private:
	InputStream*   _inputStream;
	OutputStream*  _outputStream;

	Frame*         _sourceBuffer;
	Frame*         _frameBuffer;

	InputEssence*  _inputEssence;
	OutputEssence* _outputEssence;

	EssenceTransform* _transform;

	bool _transcodeStream;
	bool _dummyStream;
};
	
}

#endif
