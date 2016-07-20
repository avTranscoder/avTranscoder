#ifndef _AV_TRANSCODER_FORMAT_CONTEXT_HPP_
#define _AV_TRANSCODER_FORMAT_CONTEXT_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/Option.hpp>

extern "C" {
#include <libavformat/avformat.h>
}

namespace avtranscoder
{

/**
 * @brief Wrapper of an AVFormatContext.
 */
class AvExport FormatContext
{
private:
    FormatContext(const FormatContext& formatContext);
    FormatContext& operator=(const FormatContext& formatContext);

public:
    /**
     * @brief Allocate an AVFormatContext by opening an input file
     */
    FormatContext(const std::string& filename, int req_flags = 0, AVDictionary** options = NULL);

    /**
     * @brief Allocate an AVFormatContext with default values
     */
    FormatContext(int req_flags = 0);

    ~FormatContext();

    /**
     * @brief Read packets of a media file to get stream information
     */
    void findStreamInfo(AVDictionary** options = NULL);

    /**
     * @brief Create and initialize a AVIOContext for accessing the resource indicated by url
     * @param url: url of ressource
     * @param flags: AVIO_FLAG_READ / AVIO_FLAG_WRITE / AVIO_FLAG_READ_WRITE
     */
    void openRessource(const std::string& url, int flags);

    /**
     * @brief Close the resource accessed by the AVIOContext and free it
     * @note Should be called after openRessource
     */
    void closeRessource();

    /**
     * @brief Write the stream header to an output media file
     * @note Also load options specific to the output format
     */
    void writeHeader(AVDictionary** options = NULL);

    /**
     * @brief Write a packet to an output media file
     * @param packet: packet to write (must be allocate and free by the caller
     * @param interleaved: to ensuring correct interleaving (available by default)
     */
    void writeFrame(AVPacket& packet, bool interleaved = true);

    /**
     * @brief Write the stream trailer to an output media file
     * @note Should be called after writeHeader
     */
    void writeTrailer();

    void addMetaData(const std::string& key, const std::string& value);
    AVStream& addAVStream(const AVCodec& avCodec);

    /**
     * @brief Seek at a specific position
     * @param position: can be in AV_TIME_BASE units, in frames... depending on the flag value
     * @param flag: seeking mode (AVSEEK_FLAG_xxx)
     * @return seek status
     * @warn seeking on a raw bitstreams (without any container) could produce an error (because of a lack of timing
     * information)
     * @see flushDecoder
     */
    bool seek(const uint64_t position, const int flag);

    size_t getNbStreams() const { return _avFormatContext->nb_streams; }
    /// Get duration of the program, in seconds
    size_t getDuration() const { return _avFormatContext->duration; }
    size_t getStartTime() const { return _avFormatContext->start_time; }

    OptionArray getOptions(); ///< Get options as array
#ifndef SWIG
    OptionMap& getOptionsMap() { return _options; } ///< Get options as map
#endif
    Option& getOption(const std::string& optionName) { return _options.at(optionName); }

    /**
     * Guess format from arguments.
     * Set the AVOutputFormat of AVFormatContext.
     * @param filename: checks if it terminates with the extensions of the registered formats
     * @param shortName: checks if it matches with the names of the registered formats
     * @param mimeType: checks if it matches with the MIME type of the registered formats
     */
    void setOutputFormat(const std::string& filename, const std::string& shortName = "", const std::string& mimeType = "");

    /**
     * Set filename of AVFormatContext.
     */
    void setFilename(const std::string& filename);

#ifndef SWIG
    AVFormatContext& getAVFormatContext() const { return *_avFormatContext; }
    AVOutputFormat& getAVOutputFormat() const { return *_avFormatContext->oformat; }
    AVInputFormat& getAVInputFormat() const { return *_avFormatContext->iformat; }
    AVIOContext& getAVIOContext() const { return *_avFormatContext->pb; }
    AVDictionary& getAVMetaData() const { return *_avFormatContext->metadata; }
    AVStream& getAVStream(size_t index) const;
#endif

private:
    AVFormatContext* _avFormatContext; ///< Has ownership
    const int _flags;                  ///< Flags with which the options are loaded (see AV_OPT_FLAG_xxx)
    OptionMap _options;
    bool _isOpen; ///< Is the AVFormatContext open (in constructor with a filename)
};
}

#endif
