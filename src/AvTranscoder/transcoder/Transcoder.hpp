#ifndef _AV_TRANSCODER_TRANSCODER_HPP_
#define _AV_TRANSCODER_TRANSCODER_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/file/IOutputFile.hpp>
#include <AvTranscoder/stream/IInputStream.hpp>
#include <AvTranscoder/profile/ProfileLoader.hpp>
#include <AvTranscoder/stat/ProcessStat.hpp>

#include "StreamTranscoder.hpp"

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

    /**
     * @brief Add all streams of the file with the given filename.
     * All the streams will be rewrapped.
     * @note Currently we rewrap only the video and the audio streams. The streams with an other type are skipped.
     */
    void add(const std::string& filename);

    /**
     * @brief Add a stream and set a profile
     * @note If profileName is empty, rewrap.
     * @note offset in seconds
     * If offset is positive, the transcoder will generate black images or silence (depending on the type of stream) before
     * the stream to process.
     * If offset is negative, the transcoder will seek in the stream and start process at this specific time.
     */
    void add(const std::string& filename, const size_t streamIndex, const std::string& profileName = "",
             const float offset = 0);
    /*
     * @note If filename is empty, add a generated stream.
     * @note If filename is empty, profileName can't be empty (no sens to rewrap a generated stream).
     */
    void add(const std::string& filename, const size_t streamIndex, const std::string& profileName, ICodec& codec,
             const float offset = 0);

    /**
     * @brief Add a stream and set a custom profile
     * @note Profile will be updated, be sure to pass unique profile name.
     */
    void add(const std::string& filename, const size_t streamIndex, const ProfileLoader::Profile& profile,
             const float offset = 0);
    /*
     * @note If filename is empty, add a generated stream.
     */
    void add(const std::string& filename, const size_t streamIndex, const ProfileLoader::Profile& profile, ICodec& codec,
             const float offset = 0);

    /**
     * @brief Add a stream and set a profile
     * @note If profileName is empty, rewrap.
     * @note If subStreamIndex is negative, no substream is selected it's the stream.
     */
    void add(const std::string& filename, const size_t streamIndex, const int subStreamIndex,
             const std::string& profileName = "", const float offset = 0);
    /**
     * @note If filename is empty, add a generated stream.
     * @note If filename is empty, profileName can't be empty (no sens to rewrap a generated stream).
     */
    void add(const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName,
             ICodec& codec, const float offset = 0);

    /**
     * @brief Add a stream and set a custom profile
     * @note Profile will be updated, be sure to pass unique profile name.
     * @note If subStreamIndex is negative, no substream is selected it's the stream.
     */
    void add(const std::string& filename, const size_t streamIndex, const int subStreamIndex,
             const ProfileLoader::Profile& profile, const float offset = 0);
    /**
     * @note If filename is empty, add a generated stream.
     */
    void add(const std::string& filename, const size_t streamIndex, const int subStreamIndex,
             const ProfileLoader::Profile& profile, ICodec& codec, const float offset = 0);

    /**
     * @brief Add the stream
     */
    void add(StreamTranscoder& streamTranscoder);

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
    void addRewrapStream(const std::string& filename, const size_t streamIndex, const float offset);

    void addTranscodeStream(const std::string& filename, const size_t streamIndex, const int subStreamIndex,
                            const float offset);
    void addTranscodeStream(const std::string& filename, const size_t streamIndex, const int subStreamIndex,
                            const ProfileLoader::Profile& profile, const float offset = 0);

    void addDummyStream(const ProfileLoader::Profile& profile, const ICodec& codec);

    /**
     * @note If streamIndex is negative, activate all streams of the file.
     */
    InputFile* addInputFile(const std::string& filename, const int streamIndex, const float offset);

    ProfileLoader::Profile getProfileFromFile(InputFile& inputFile,
                                              const size_t streamIndex); ///< The function analyses the inputFile

    /**
     * @brief Get the duration of the stream, in seconds
     * @note If the stream is a generator, return limit of double.
     */
    float getStreamDuration(size_t indexStream) const;

    /**
    * @brief Get the duration of the shortest stream, in seconds
    */
    float getMinTotalDuration() const;

    /**
     * @brief Get the duration of the longest stream, in seconds
     */
    float getMaxTotalDuration() const;

    /**
     * @brief Get the expected duration of the output program
     * @note Depends on the streams, the process method, and the main stream index.
     */
    float getExpectedOutputDuration() const;

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

    ProfileLoader _profileLoader;   ///< Objet to get existing profiles, and add new ones for the Transcoder.

    EProcessMethod _eProcessMethod; ///< Processing policy
    size_t
        _mainStreamIndex;  ///< Index of stream used to stop the process of transcode in case of eProcessMethodBasedOnStream.
    float _outputDuration; ///< Duration of output media used to stop the process of transcode in case of
                           ///eProcessMethodBasedOnDuration.
};
}

#endif
