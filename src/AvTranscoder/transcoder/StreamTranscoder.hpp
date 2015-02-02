#ifndef _AV_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _AV_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/stream/IInputStream.hpp>
#include <AvTranscoder/stream/IOutputStream.hpp>

#include <AvTranscoder/decoder/IDecoder.hpp>
#include <AvTranscoder/encoder/IEncoder.hpp>

#include <AvTranscoder/file/IOutputFile.hpp>

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
	StreamTranscoder( IInputStream& inputStream, IOutputFile& outputFile );

	/**
	 * @brief transcode stream
	 **/
	StreamTranscoder( IInputStream& inputStream, IOutputFile& outputFile, const ProfileLoader::Profile& profile, const int subStreamIndex = -1, const size_t offset = 0 );

	/**
	 * @brief encode from a generated stream
	 * @note offset feature has no sense here
	 **/
	StreamTranscoder( const ICodec& inputCodec, IOutputFile& outputFile, const ProfileLoader::Profile& profile );

	~StreamTranscoder();

	/**
	 * @brief Pre-process codec latency.
	 * @note This can be called several times with no side effects.
	 * @note Can take a little bit of time.
	 */
	void preProcessCodecLatency();
	
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

	/// Returns a reference to the current decoder used by the streamTranscoder (from input file or from generator)
	IDecoder& getCurrentDecoder() const { return *_currentDecoder; }

	void setVerbose( bool verbose = true ){ _verbose = verbose; }

	void setInfinityStream( bool isInfinity ) { _infinityStream = isInfinity; }

	void setOffset( bool offset = true ){ _offset = offset; }

private:
	bool processRewrap();
	bool processTranscode( const int subStreamIndex = -1 );  ///< By default transcode all channels

private:
	IInputStream*   _inputStream;
	IOutputStream*  _outputStream;

	Frame*         _sourceBuffer;
	Frame*         _frameBuffer;

	IDecoder* _inputDecoder;
	IDecoder* _generator;
	IDecoder* _currentDecoder;
	IEncoder* _outputEncoder;

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
