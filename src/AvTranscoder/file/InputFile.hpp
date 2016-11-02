#ifndef _AV_TRANSCODER_FILE_INPUT_FILE_HPP_
#define _AV_TRANSCODER_FILE_INPUT_FILE_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/file/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>
#include <AvTranscoder/stream/InputStream.hpp>
#include <AvTranscoder/properties/FileProperties.hpp>
#include <AvTranscoder/progress/IProgress.hpp>
#include <AvTranscoder/profile/ProfileLoader.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

class AvExport InputFile
{
private:
    InputFile(const InputFile& inputFile);
    InputFile& operator=(const InputFile& inputFile);

public:
    /**
     * @brief Open a media file
     * @note The constructor also analyses header of input file
     * @param filename resource to access
     * @exception ios_base::failure launched if unable to open file
    **/
    InputFile(const std::string& filename);

    virtual ~InputFile();

    /**
     * @brief Run the analyse on the file after a setup.
     *        call this function before getProperties().
     * @param progress callback to get analysis progression
     * @param level by default eAnalyseLevelFirstGop
     **/
    void analyse(IProgress& progress, const EAnalyseLevel level = eAnalyseLevelFirstGop);

    /**
     * @brief Read the next packet of the specified stream
     * @param data: data of next packet read
     * @return if next packet was read succefully
     **/
    bool readNextPacket(CodedData& data, const size_t streamIndex);

    /**
     * @brief Seek at a specific frame / time (in seconds)
     * @note Seek in file by using the default stream (according to ffmpeg)
     * @param flag: ffmpeg seek flag (by default seek to any frame, even non-keyframes)
     * @warning If the seek is done to a non key-frame, the decoding will start from the next key-frame
     * @return seek status
     **/
    bool seekAtFrame(const size_t frame, const int flag = AVSEEK_FLAG_ANY);
    bool seekAtTime(const double time, const int flag = AVSEEK_FLAG_ANY);

    /**
     * @brief Activate the indicated stream
     * @note Activate a stream results in buffered its data when processing
     **/
    void activateStream(const size_t streamIndex, const bool activate = true);

    /**
     * @brief Return media properties on the current InputFile.
     * @note require to launch analyse() before to fill the property struture
     * @return structure of media metadatas
     **/
    const FileProperties& getProperties() const { return *_properties; }

    /**
     * @brief Get stream type: video, audio, subtitle, etc.
     * @param index stream index
     * @return media stream type of specified index stream
     **/
    InputStream& getStream(size_t index);

    std::string getFilename() const { return _filename; }

    const FormatContext& getFormatContext() const { return _formatContext; }

    /**
     * @brief Set the format of the input file
     * @param profile: the profile of the input format
     */
    virtual void setupUnwrapping(const ProfileLoader::Profile& profile);

private:
    /**
     * @brief Get Fps from first video stream
     * @note if there is no video stream, return 1.
     */
    double getFps();

protected:
    FormatContext _formatContext;
    FileProperties* _properties;
    std::string _filename;
    std::vector<InputStream*> _inputStreams; ///< Has ownership
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const InputFile& input);
#endif
}

#endif
