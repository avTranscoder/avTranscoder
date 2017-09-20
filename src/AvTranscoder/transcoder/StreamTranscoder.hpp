#ifndef _AV_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _AV_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/transcoder/InputStreamDesc.hpp>

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
     **/
    StreamTranscoder(IInputStream& inputStream, IOutputFile& outputFile, const float offset = 0);

    /**
     * @brief Transcode the given streams.
     * @note The data are wrapped to one output stream.
     **/
    StreamTranscoder(const std::vector<InputStreamDesc>& inputStreamsDesc, std::vector<IInputStream*>& inputStreams, IOutputFile& outputFile, 
                     const ProfileLoader::Profile& profile, const float offset = 0);

    /**
     * @brief Encode a generated stream
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
    // Switch current decoder.
    void switchToGeneratorDecoder();
    void switchToInputDecoder();
    //@}

    /**
     * @brief Get the total duration (in seconds), ie. duration of the shortest input stream and the offset applies
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
    std::vector<IInputStream*> getInputStreams() const { return _inputStreams; }
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
    // @brief Get the current process case.
    // @warning Could vary during the process.
    enum EProcessCase
    {
        eProcessCaseTranscode,
        eProcessCaseRewrap,
        eProcessCaseGenerator
    };
    EProcessCase getProcessCase() const;
    //@}

private:
    /**
     * @brief Create the decoder (and the other related objects needed) which decodes the given input stream.
     * @param inputStreamDesc
     * @param inputStream
     */
    void addDecoder(const InputStreamDesc& inputStreamDesc, IInputStream& inputStream);
    void addGenerator(const InputStreamDesc& inputStreamDesc, const ProfileLoader::Profile& profile);

    bool processRewrap();
    bool processTranscode();

    size_t getFirstInputStreamIndex();

private:
    std::vector<InputStreamDesc> _inputStreamDesc; ///< Description of the data to extract from the input stream.
    std::vector<IInputStream*> _inputStreams;   ///< List of input stream to read next packet (has link, no ownership)
    IOutputStream* _outputStream; ///< Output stream to wrap next packet (has link, no ownership)

    std::vector<IFrame*> _decodedData; ///< List of buffers of decoded data (has ownership).
    IFrame* _filteredData;  ///< Buffer of filtered data (has ownership).
    IFrame* _transformedData;  ///< Buffer of transformed data (has ownership).

    std::vector<IDecoder*> _inputDecoders;   ///< Decoders of packets read from _inputStream (has ownership)
    std::vector<IDecoder*> _generators;      ///< Generators of audio or video packets (has ownership)
    IDecoder* _currentDecoder; ///< Link to _inputDecoder or _generator
    IEncoder* _outputEncoder;  ///< Encoder of packets which will be wrapped by _outputStream (has ownership)

    ITransform* _transform; ///< Video or audio transform (has ownership)

    FilterGraph* _filterGraph; ///< Filter graph (has ownership)

    float _offset; ///< Offset, in seconds, at the beginning of the StreamTranscoder.

    bool _needToSwitchToGenerator; ///< Set if need to switch to a generator during the process (because, of other streams
    /// duration, or an offset)
};
}

#endif
