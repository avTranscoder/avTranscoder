#ifndef _AV_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _AV_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/CodedStream/InputStream.hpp>

#include <AvTranscoder/EssenceStream/InputVideo.hpp>
#include <AvTranscoder/EssenceStream/OutputVideo.hpp>

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
	StreamTranscoder( InputStream& inputStream, OutputFile& outputFile, const Profile::ProfileDesc& profile, const int subStreamIndex = -1 );

	/**
	 * @brief encode from dummy stream
	 **/
	StreamTranscoder( InputEssence& inputEssence, OutputFile& outputFile, const Profile::ProfileDesc& profile );

	~StreamTranscoder();

	/**
	 * @brief process a single frame for the current stream
	 * @return the process status result
	 */
	bool processFrame();

	bool isTranscodeStream() const { return _transcodeStream; }

	void setVerbose( bool verbose = true ){ _verbose = verbose; }

private:
	bool processRewrap();
	bool processRewrap( const int subStreamIndex );
	bool processTranscode();
	bool processTranscode( const int subStreamIndex );

private:
	InputStream*   _inputStream;
	OutputStream*  _outputStream;

	Frame*         _sourceBuffer;
	Frame*         _frameBuffer;

	InputEssence*  _inputEssence;
	OutputEssence* _outputEssence;

	EssenceTransform* _transform;

	int  _subStreamIndex;
	bool _transcodeStream;
	bool _verbose;
};
	
}

#endif
