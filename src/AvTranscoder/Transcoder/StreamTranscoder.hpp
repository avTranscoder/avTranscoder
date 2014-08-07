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
	 * @note offset feature when rewrap a stream is not supported
	 **/
	StreamTranscoder( InputStream& inputStream, OutputFile& outputFile );

	/**
	 * @brief transcode stream
	 **/
	StreamTranscoder( InputStream& inputStream, OutputFile& outputFile, const Profile::ProfileDesc& profile, const int subStreamIndex = -1, const size_t offset = 0 );

	/**
	 * @brief encode from dummy stream
	 * @note offset feature has no sense here
	 **/
	StreamTranscoder( InputEssence& inputEssence, OutputFile& outputFile, const Profile::ProfileDesc& profile );

	~StreamTranscoder();

	/**
	 * @brief process a single frame for the current stream
	 * @return the process status result
	 */
	bool processFrame();

	void switchEssence( bool swithToDummy = true );
	void switchToDummyEssence();
	void switchToInputEssence();

	void setVerbose( bool verbose = true ){ _verbose = verbose; }

	void setInfinityStream( bool isInfinity ) { _infinityStream = isInfinity; }

	void setOffset( bool offset = true ){ _offset = offset; }

	/**
     * @brief Get the duration of the stream.
	 * @note if it's a dummy stream, return limit of double.
     */
	double getDuration() const;

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
	InputEssence*  _dummyEssence;
	InputEssence*  _currentEssence;
	OutputEssence* _outputEssence;

	EssenceTransform* _transform;

	int  _subStreamIndex;

	/**
	 * @brief How many frame processed for this StreamTranscoder.
	 */
	size_t _frameProcessed;
	/**
	 * @brief Offset, in frame, at the beginning of the StreamTranscoder.
	 */
	size_t _offset;

	bool _takeFromDummy;

	bool _verbose;

	bool _offsetPassed;

	/**
	 * @brief Automatic switch to dummy
	 * @note not applicable when rewrap
	 */
	bool _infinityStream;
};
	
}

#endif
