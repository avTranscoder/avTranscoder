#ifndef _AV_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _AV_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/stream/IInputStream.hpp>
#include <AvTranscoder/stream/IOutputStream.hpp>

#include <AvTranscoder/decoder/IDecoder.hpp>
#include <AvTranscoder/encoder/IEncoder.hpp>

#include <AvTranscoder/file/IOutputFile.hpp>

#include <AvTranscoder/profile/ProfileLoader.hpp>

namespace avtranscoder
{

class ITransform;

class AvExport StreamTranscoder
{
private:
	StreamTranscoder( const StreamTranscoder& streamTranscoder );
	StreamTranscoder& operator=( const StreamTranscoder& streamTranscoder );

public:
	/**
	 * @brief rewrap stream
	 * @note offset feature when rewrap a stream is not supported
	 **/
	StreamTranscoder( IInputStream& inputStream, IOutputFile& outputFile, const double offset = 0 );

	/**
	 * @brief transcode stream
	 **/
	StreamTranscoder( IInputStream& inputStream, IOutputFile& outputFile, const ProfileLoader::Profile& profile, const int subStreamIndex = -1, const double offset = 0 );

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

	//@{
	/** Switch decoder */
	void switchToGeneratorDecoder();
	void switchToInputDecoder();
	//@}

	/**
	 * @brief Get the duration of the stream, in seconds
	 * @note if it's a generated stream, return limit of double.
	 */
	double getDuration() const;

	/// Returns a reference to the current decoder (from input file or from generator)
	IDecoder& getCurrentDecoder() const { return *_currentDecoder; }
	/// Returns a reference to the encoder
	IEncoder& getEncoder() const { return *_outputEncoder; }

	/// Returns a reference to the object which transforms the decoded data
	ITransform& getTransform() const { return *_transform; }

	/// Returns a reference to the stream which unwraps data
	IInputStream& getInputStream() const { return *_inputStream; }
	/// Returns a reference to the stream which wraps data
	IOutputStream& getOutputStream() const { return *_outputStream; }

	/**
	 * @brief Returns if the stream can switch to a generator when ended
	 * @note Not applicable for rewrap and generator cases
	 */
	void canSwitchToGenerator( bool canSwitch ) { _canSwitchToGenerator = canSwitch; }

	void setOffset( bool offset = true ){ _offset = offset; }

private:
	bool processRewrap();
	bool processTranscode( const int subStreamIndex = -1 );  ///< By default transcode all channels

	bool isTranscodeCase() const;
	bool isRewrapCase() const;
	bool isGeneratorCase() const;

private:
	IInputStream* _inputStream;  ///< Input stream to read next packet (has link, no ownership)
	IOutputStream* _outputStream;  ///< Output stream to wrap next packet (has link, no ownership)

	Frame* _sourceBuffer;  ///< Has ownership
	Frame* _frameBuffer;  ///< Has ownership

	IDecoder* _inputDecoder;  ///< Decoder of packets read from _inputStream (has ownership)
	IDecoder* _generator;  ///< Generator of audio or video packets (has ownership)
	IDecoder* _currentDecoder;  ///< Link to _inputDecoder or _generator
	IEncoder* _outputEncoder;  ///< Encoder of packets which will be wrapped by _outputStream (has ownership)

	ITransform* _transform;  ///< Video or audio transform (has ownership)

	int  _subStreamIndex;  ///< Index of channel that is processed from the input stream (<0 if no demultiplexing).

	double _offset;  ///< Offset, in seconds, at the beginning of the StreamTranscoder.

	bool _canSwitchToGenerator;  ///< Automatically switch to a generator at the end of the stream
};

}

#endif
