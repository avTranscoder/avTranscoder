#ifndef _AV_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _AV_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/codedStream/IInputStream.hpp>
#include <AvTranscoder/codedStream/IOutputStream.hpp>

#include <AvTranscoder/essenceStream/AvInputVideo.hpp>
#include <AvTranscoder/essenceStream/AvOutputVideo.hpp>

#include <AvTranscoder/file/OutputFile.hpp>

#include <AvTranscoder/ProfileLoader.hpp>

namespace avtranscoder
{

class ITransform;

class AvExport StreamTranscoder
{
public:
	/**
	 * @brief rewrap stream
	 * @note offset feature when rewrap a stream is not supported
	 **/
	StreamTranscoder( IInputStream& inputStream, OutputFile& outputFile );

	/**
	 * @brief transcode stream
	 **/
	StreamTranscoder( IInputStream& inputStream, OutputFile& outputFile, const ProfileLoader::Profile& profile, const int subStreamIndex = -1, const size_t offset = 0 );

	/**
	 * @brief encode from a generated stream
	 * @note offset feature has no sense here
	 **/
	StreamTranscoder( IInputEssence& inputEssence, OutputFile& outputFile, const ProfileLoader::Profile& profile );

	~StreamTranscoder();

	/**
	 * @brief Init before encoding to pre-process frames necessary to delete the latency.
	 * @note This can be called several times with no side effects.
	 * @note Can take a little bit of time.
	 */
	void init();
	
	/**
	 * @brief process a single frame for the current stream
	 * @return the process status result
	 */
	bool processFrame();

	void switchEssence( bool swithToGenerator = true );
	void switchToGeneratorEssence();
	void switchToInputEssence();

	/**
	 * @brief Get the duration of the stream.
	 * @note if it's a generated stream, return limit of double.
	 */
	double getDuration() const;

	/**
	 * @return a reference to the current essence processed in this stream.
	 */
	IInputEssence& getCurrentEssence() const { return *_currentEssence; }

	void setVerbose( bool verbose = true ){ _verbose = verbose; }

	void setInfinityStream( bool isInfinity ) { _infinityStream = isInfinity; }

	void setOffset( bool offset = true ){ _offset = offset; }

private:
	bool processRewrap();
	bool processTranscode();
	bool processTranscode( const int subStreamIndex );

private:
	IInputStream*   _inputStream;
	IOutputStream*  _outputStream;

	Frame*         _sourceBuffer;
	Frame*         _frameBuffer;

	IInputEssence*  _inputEssence;
	IInputEssence*  _generatorEssence;
	IInputEssence*  _currentEssence;
	IOutputEssence* _outputEssence;

	ITransform* _transform;

	int  _subStreamIndex;  ///< Index of channel that is processed from the input stream (-1 if no demultiplexing).

	size_t _frameProcessed;  ///< How many frame processed for this StreamTranscoder.
	size_t _offset;  ///< Offset, in frame, at the beginning of the StreamTranscoder.

	bool _takeFromGenerator;  ///< Is the data processed are taken from a generator.
	bool _verbose;
	bool _offsetPassed;  ///< Is the offset at the beginning of the stream is finished.
	bool _infinityStream;  ///< Automatically switch to a generator at the end of the stream (not applicable when rewrap);
};

}

#endif
