#ifndef _AV_TRANSCODER_FILE_OUTPUT_FILE_HPP_
#define _AV_TRANSCODER_FILE_OUTPUT_FILE_HPP_

#include <AvTranscoder/file/IOutputFile.hpp>

#include <AvTranscoder/properties/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>

#include <vector>

namespace avtranscoder
{

/**
 * @brief Outputfile is the default implentation of wrapper which uses LibAV/FFMpeg.
 **/
class AvExport OutputFile : public IOutputFile
{
private:
    OutputFile(const OutputFile& outputFile);
    OutputFile& operator=(const OutputFile& outputFile);

public:
    /**
     * @brief Create an output media file.
     * @param filename resource to access
     * @param formatName should matches with the names of the registered formats
     * @param mimeType should matches with the MIME type of the registered formats
     * @note The caller should indicate formatName and/or mimeType if the filename has no extension.
     * @note The ressource is allocated when beginWrap.
     * @see beginWrap
    **/
    OutputFile(const std::string& filename, const std::string& formatName = "", const std::string& mimeType = "");

    ~OutputFile();

    IOutputStream& addVideoStream(const VideoCodec& videoDesc);
    IOutputStream& addAudioStream(const AudioCodec& audioDesc);
    IOutputStream& addDataStream(const DataCodec& dataDesc);
    IOutputStream& addCustomStream(const ICodec& iCodecDesc);

    /**
     * @brief Open ressource, write header, and setup specific wrapping options given when call setupWrapping.
     * @note Need to add the streams to mux before calling this method.
     * @note After this call, a new list of AVOption, relative to the format choosen, will be available for the OutputFile.
     */
    bool beginWrap();

    IOutputStream::EWrappingStatus wrap(const CodedData& data, const size_t streamIndex);

    /**
     * @brief Close ressource and write trailer.
     */
    bool endWrap();

    /**
     * @brief Add metadata to the output file.
     * @note Depending on the format, you are not sure to find your metadata after the transcode.
     */
    void addMetadata(const PropertyVector& data);
    void addMetadata(const std::string& key, const std::string& value);

    IOutputStream& getStream(const size_t streamIndex);

    std::string getFilename() const;

    /**
     * @brief A comma separated list of short names for the format, or empty if unknown.
     */
    std::string getFormatName() const;

    /**
     * @brief Descriptive name for the format, meant to be more human-readable than name, or empty if unknown.
     */
    std::string getFormatLongName() const;

    /**
     * @brief Comma-separated list of mime types, or empty if unknown.
     */
    std::string getFormatMimeType() const;

    FormatContext& getFormatContext() { return _formatContext; }

    /**
     * @brief Set the format and the generic options of the output file.
     * @param profile: the profile of the output format
     * @note options specific to the output format will be set in beginWrap.
     * @see beginWrap
     */
    void setupWrapping(const ProfileLoader::Profile& profile);

private:
    //@{
    // @brief Set options of output format.
    // @note setupWrappingOptions: called when setupWrapping, to set common options.
    // @note setupRemainingWrappingOptions: called when beginWrap, to set specific options.
    // @see setupWrapping
    void setupWrappingOptions(const ProfileLoader::Profile& profile);
    void setupRemainingWrappingOptions();
    //@}

    /**
     * @brief Depending on the format/codec, set general parameters of the stream.
     * @param avStream: the stream to set.
     * @param codec; the description of the codec used in the stream.
     */
    void setOutputStream(AVStream& avStream, const ICodec& codec);

private:
    FormatContext _formatContext;
    std::vector<OutputStream*> _outputStreams; ///< Has ownership
    std::vector<size_t> _frameCount;           ///< Number of wrapped frames

    double _previousProcessedStreamDuration; ///< To manage process streams order

    /**
     * @brief To setup specific wrapping options.
     * @see setupWrapping
     * @see beginWrap
     */
    AVDictionary* _profileOptions;
};
}

#endif
