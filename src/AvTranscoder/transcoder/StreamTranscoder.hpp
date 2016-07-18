#ifndef _AV_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _AV_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/stream/IInputStream.hpp>
#include <AvTranscoder/stream/IOutputStream.hpp>

#include <AvTranscoder/decoder/IDecoder.hpp>
#include <AvTranscoder/encoder/IEncoder.hpp>

#include <AvTranscoder/file/IOutputFile.hpp>
#include <AvTranscoder/filter/FilterGraph.hpp>
#include <AvTranscoder/profile/ProfileLoader.hpp>

namespace avtranscoder
{

class ITransform;

class AvExport StreamTranscoder
{
private:
    StreamTranscoder(const StreamTranscoder& streamTranscoder);
    StreamTranscoder& operator=(const StreamTranscoder& streamTranscoder);

public:
    /**
     * @brief Rewrap the given stream.
     * @note offset feature when rewrap a stream is not supported
     **/
    StreamTranscoder(IInputStream& inputStream, IOutputFile& outputFile, const float offset = 0);

    /**
     * @brief Transcode the given stream.
     **/
    StreamTranscoder(IInputStream& inputStream, IOutputFile& outputFile, const ProfileLoader::Profile& profile,
                     const std::vector<size_t> channelsIndex, const float offset = 0);

    /**
     * @brief encode from a generated stream
     * @note offset feature has no sense here
     **/
    StreamTranscoder(IOutputFile& outputFile, const ProfileLoader::Profile& profile);

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
     * @brief Get the total duration (in seconds), ie. duration of the stream and the offset applies
     * @note if it's a generated stream, return limit of double.
     * @note if offset > duration of the stream, return 0
     */
    float getDuration() const;

    /// Returns a pointer to the current decoder (from input file or from generator)
    IDecoder* getCurrentDecoder() const { return _currentDecoder; }
    /// Returns a pointer to the encoder
    IEncoder* getEncoder() const { return _outputEncoder; }

    /// Returns a pointer to the object which transforms the decoded data
    ITransform* getTransform() const { return _transform; }

    /// Returns a pointer to the object which manage filtering
    FilterGraph* getFilterGraph() const { return _filterGraph; }

    /// Returns a pointer to the stream which unwraps data
    IInputStream* getInputStream() const { return _inputStream; }
    /// Returns a reference to the stream which wraps data
    IOutputStream& getOutputStream() const { return *_outputStream; }

    /**
     * @brief Returns if the stream has the ability to switch to a generator.
     */
    bool canSwitchToGenerator();

    /**
     * @brief Set if the stream needs to switch to a generator when ended
     * @note Throws a runtime_error exception if the stream cannot switch to a generator
     * @see canSwitchToGenerator
     */
    void needToSwitchToGenerator(const bool needToSwitch = true);

    /**
     * @note Throws a runtime_error exception if it's a positive offset and the stream cannot switch to a generator
     * @see needToSwitchToGenerator
     */
    void setOffset(const float offset);

    //@{
    // Get the current process case.
    enum EProcessCase
    {
        eProcessCaseTranscode,
        eProcessCaseRewrap,
        eProcessCaseGenerator
    };
    EProcessCase getProcessCase() const;
    //@}

private:
    bool processRewrap();
    bool processTranscode();

private:
    IInputStream* _inputStream;   ///< Input stream to read next packet (has link, no ownership)
    IOutputStream* _outputStream; ///< Output stream to wrap next packet (has link, no ownership)

    Frame* _sourceBuffer; ///< Has ownership
    Frame* _frameBuffer;  ///< Has ownership

    IDecoder* _inputDecoder;   ///< Decoder of packets read from _inputStream (has ownership)
    IDecoder* _generator;      ///< Generator of audio or video packets (has ownership)
    IDecoder* _currentDecoder; ///< Link to _inputDecoder or _generator
    IEncoder* _outputEncoder;  ///< Encoder of packets which will be wrapped by _outputStream (has ownership)

    ITransform* _transform; ///< Video or audio transform (has ownership)

    FilterGraph* _filterGraph; ///< Filter graph (has ownership)

    std::vector<size_t> _channelsIndex; ///< List of channels that is processed from the input stream (empty if no demultiplexing).

    float _offset; ///< Offset, in seconds, at the beginning of the StreamTranscoder.

    bool _needToSwitchToGenerator; ///< Set if need to switch to a generator during the process (because, of other streams
    /// duration, or an offset)
};
}

#endif
