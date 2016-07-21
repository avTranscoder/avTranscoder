#ifndef _AV_TRANSCODER_TRANSCODER_HPP_
#define _AV_TRANSCODER_TRANSCODER_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/file/IOutputFile.hpp>
#include <AvTranscoder/stream/IInputStream.hpp>
#include <AvTranscoder/profile/ProfileLoader.hpp>
#include <AvTranscoder/stat/ProcessStat.hpp>

#include <AvTranscoder/transcoder/InputStreamDesc.hpp>
#include <AvTranscoder/transcoder/StreamTranscoder.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

/**
 * @brief Enum to set a policy of how we manage the process in case of several streams.
 * eProcessMethodShortest: stop the process at the end of the shortest stream.
 * eProcessMethodLongest: stop the process at the end of the longest stream.
 * eProcessMethodBasedOnStream: stop the process at the end of an indicated stream (@see _indexBasedStream attribute of
 * Transcoder).
 * eProcessMethodBasedOnDuration: stop the process at the end of an indicated duration, in seconds (@see _outputDuration
 * attribute of Transcoder).
 * eProcessMethodInfinity: stop the process by outside of avTranscoder (streaming mode)
 */
enum EProcessMethod
{
    eProcessMethodShortest = 0,
    eProcessMethodLongest,
    eProcessMethodBasedOnStream,
    eProcessMethodBasedOnDuration,
    eProcessMethodInfinity,
};

/**
 * @brief A Transcoder manages a list of streams,
 * and process a transcode to create an output media file.
 */
class AvExport Transcoder
{
private:
    Transcoder(const Transcoder& transcoder);
    Transcoder& operator=(const Transcoder& transcoder);

public:
    /**
     * @note Set FFmpeg log level to quite.
     */
    Transcoder(IOutputFile& outputFile);

    ~Transcoder();

    //@{
    // @brief Add a new stream to the output file, created from the given input description to process.
    // @param profileName: the encoding profile (rewrap if empty)
    // @param offset: in seconds
    // If offset is positive, the transcoder will generate black images or silence (depending on the type of stream) before
    // the stream to process.
    // If offset is negative, the transcoder will seek in the stream and start process at this specific time.
    void addStream(const InputStreamDesc& inputStreamDesc, const std::string& profileName = "", const float offset = 0);
    void addStream(const InputStreamDesc& inputStreamDesc, const ProfileLoader::Profile& profile, const float offset = 0);
    //@}

    //@{
    // @brief Add a new stream to the output file, created from the given input description to process.
    // @param inputStreamDescArray: the type of the described streams should be of the same type.
    void addStream(const std::vector<InputStreamDesc>& inputStreamDescArray, const std::string& profileName, float offset = 0);
    void addStream(const std::vector<InputStreamDesc>& inputStreamDescArray, const ProfileLoader::Profile& profile, const float offset = 0);
    //@}

    //@{
    // @brief Add a new generated stream to the output file, created from the given encoding profile.
    void addGenerateStream(const std::string& encodingProfileName);
    void addGenerateStream(const ProfileLoader::Profile& encodingProfile);
    //@}

    /**
     * @brief Add the stream
     */
    void addStream(StreamTranscoder& streamTranscoder);

    /**
     * @brief Initialize all added streams, processing codec latency.
     * @note This can be called several times with no side effects.
     * @note Can take a little bit of time.
     */
    void preProcessCodecLatency();

    /**
     * @brief Process the next frame of all streams.
     * @return if a frame was processed or not.
     */
    bool processFrame();

    /**
     * @brief Process all the streams, and ended the process depending on the transcode politic.
     * @note The function manages all process: init(), beginWrap(), processFrame()s, and endWrap().
     * @param progress: choose a progress, or create your own in C++ or in bindings by inherit IProgress class.
     * @return ProcessStat: object with statistics of the process for each stream.
     * @see IProgress
     */
    ProcessStat process(IProgress& progress);
    ProcessStat process(); ///< Call process with no display of progression

    /**
     * @brief Return the list of streams added to the transcoder.
     */
    std::vector<StreamTranscoder*>& getStreamTranscoders() { return _streamTranscoders; }

    /**
     * @param streamIndex: careful about the order of stream insertion of the Transcoder.
     * @return a reference to a stream manage by the Transcoder.
     */
    StreamTranscoder& getStreamTranscoder(size_t streamIndex) const { return *_streamTranscoders.at(streamIndex); }

    /**
     * @brief Get current processMethod
     * @see EProcessMethod
     */
    EProcessMethod getProcessMethod() const { return _eProcessMethod; }

    /**
     * @brief Set the transcoding policy.
     * @note By default eProcessMethodBasedOnStream at index 0.
     * @param indexBasedStream: in case of process method eProcessMethodBasedOnStream, stop transcode at the end of the
     * indicated stream.
     * @param outputDuration: in case of process method eProcessMethodBasedOnDuration, stop transcode at the end of the
     * indicated duration.
     */
    void setProcessMethod(const EProcessMethod eProcessMethod, const size_t indexBasedStream = 0,
                          const double outputDuration = 0);

private:
    void addRewrapStream(const InputStreamDesc& inputStreamDesc, const float offset);
    void addTranscodeStream(const std::vector<InputStreamDesc>& inputStreamDescArray, const ProfileLoader::Profile& profile,
                            const float offset = 0);

    /**
     * @note If streamIndex is negative, activate all streams of the file.
     */
    InputFile* addInputFile(const std::string& filename, const int streamIndex, const float offset);

    /**
     * @return The profile from the given input. 
     */
    ProfileLoader::Profile getProfileFromInput(const InputStreamDesc& inputStreamDesc);

    /**
     * @brief Get the duration of the stream, in seconds
     * @note If the stream is a generator, return limit of double.
     */
    float getStreamDuration(size_t indexStream) const;

    /**
     * @brief Get the duration of the shortest stream, in seconds
     * @note Set the index of the main stream to stop the process at the end of the shortest stream.
     */
    float getMinTotalDuration();

    /**
     * @brief Get the duration of the longest stream, in seconds
     * @note Set the index of the main stream to stop the process at the end of the longest stream.
     */
    float getMaxTotalDuration();

    /**
     * @brief Get the expected duration of the output program
     * @note Depends on the streams, the process method, and the main stream index.
     */
    float getExpectedOutputDuration();

    /**
     * @brief Get the current duration of the output program
     * @note Returns the duration of the smallest stream.
     * @return -1 if there is no output stream.
     */
    float getCurrentOutputDuration() const;

    /**
     * @brief Set for each StreamTranscoder if it can switch to generator at the end.
     */
    void manageSwitchToGenerator();

    /**
     * @brief Fill the given ProcessStat to summarize the process.
     */
    void fillProcessStat(ProcessStat& processStat);

private:
    IOutputFile& _outputFile;            ///< The output media file after process (has link)
    std::vector<InputFile*> _inputFiles; ///< The list of input files which contain added streams (has ownership)

    std::vector<StreamTranscoder*> _streamTranscoders;          ///< All streams of the output media file after process.
    std::vector<StreamTranscoder*> _streamTranscodersAllocated; ///< Streams allocated inside the Transcoder (has ownership)

    ProfileLoader _profileLoader; ///< Objet to get existing profiles, and add new ones for the Transcoder.

    EProcessMethod _eProcessMethod; ///< Processing policy
    size_t
        _mainStreamIndex;  ///< Index of stream used to stop the process.
    float _outputDuration; ///< Duration of output media used to stop the process of transcode in case of
    /// eProcessMethodBasedOnDuration.
};
}

#endif
