#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_FILE_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_FILE_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/properties/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>
#include <AvTranscoder/progress/IProgress.hpp>

#include <AvTranscoder/properties/StreamProperties.hpp>
#include <AvTranscoder/properties/VideoProperties.hpp>
#include <AvTranscoder/properties/AudioProperties.hpp>
#include <AvTranscoder/properties/DataProperties.hpp>
#include <AvTranscoder/properties/SubtitleProperties.hpp>
#include <AvTranscoder/properties/AttachementProperties.hpp>
#include <AvTranscoder/properties/UnknownProperties.hpp>

#include <string>
#include <vector>
#include <map>

namespace avtranscoder
{

class AvExport FileProperties
{
public:
    /**
     * @brief Analayse a file from its FormatContext
     * @note The default streams analyse level is eAnalyseLevelHeader
     * @see FormatContext
     */
    FileProperties(const FormatContext& formatContext);

    /**
     * @brief Relaunch streams analysis with a specific level.
     * @param progress callback to get analysis progression
     * @param level of analysis
     */
    void extractStreamProperties(IProgress& progress, const EAnalyseLevel level);

    std::string getFilename() const;
    std::string getFormatName() const; ///< A comma separated list of short names for the format, or empty if unknown.
    std::string getFormatLongName() const; ///< Descriptive name for the format, meant to be more human-readable than name, or empty if unknown.
    std::string getFormatMimeType() const; ///< Comma-separated list of mime types, or empty if unknown.

    size_t getProgramsCount() const;
    double getStartTime() const;
    float getDuration() const; ///< in seconds
    size_t getBitRate() const; ///< total stream bitrate in bit/s, 0 if not available (result of a computation by ffmpeg)
    size_t getPacketSize() const;

    const PropertyVector& getMetadatas() const { return _metadatas; }

    size_t getNbStreams() const;
    size_t getNbVideoStreams() const { return _videoStreams.size(); }
    size_t getNbAudioStreams() const { return _audioStreams.size(); }
    size_t getNbDataStreams() const { return _dataStreams.size(); }
    size_t getNbSubtitleStreams() const { return _subtitleStreams.size(); }
    size_t getNbAttachementStreams() const { return _attachementStreams.size(); }
    size_t getNbUnknownStreams() const { return _unknownStreams.size(); }

    const FormatContext& getFormatContext() { return *_formatContext; }

    //@{
    // @brief Get the properties at the indicated stream index
    // @throws A runtime error if the streamIndex does not match any stream
    const avtranscoder::StreamProperties& getStreamPropertiesWithIndex(const size_t streamIndex) const;
    //@}

    //@{
    // @brief Get the list of properties for a given type (video, audio...)
    const std::vector<avtranscoder::StreamProperties*> getStreamProperties() const;
    const std::vector<avtranscoder::VideoProperties>& getVideoProperties() const { return _videoStreams; }
    const std::vector<avtranscoder::AudioProperties>& getAudioProperties() const { return _audioStreams; }
    const std::vector<avtranscoder::DataProperties>& getDataProperties() const { return _dataStreams; }
    const std::vector<avtranscoder::SubtitleProperties>& getSubtitleProperties() const { return _subtitleStreams; }
    const std::vector<avtranscoder::AttachementProperties>& getAttachementProperties() const { return _attachementStreams; }
    const std::vector<avtranscoder::UnknownProperties>& getUnknownProperties() const { return _unknownStreams; }
//@}

#ifndef SWIG
    const AVFormatContext& getAVFormatContext() { return *_avFormatContext; }
#endif

    std::string allPropertiesAsJson() const; ///< Return all properties as a json format.
    std::string asJson() const;              ///< Return all format properties as a json format.
    PropertyMap asMap() const;               ///< Return format properties as a map (name of property, value)
    PropertyVector asVector() const;         ///< Return format properties as a vector (name of property: value)
    PropertyVector& fillVector(PropertyVector& data) const; ///< To avoid copy of the vector

private:
#ifndef SWIG
    template <typename T>
    void addProperty(PropertyVector& data, const std::string& key, T (FileProperties::*getter)(void) const) const
    {
        try
        {
            detail::add(data, key, (this->*getter)());
        }
        catch(const std::exception& e)
        {
            detail::add(data, key, detail::propertyValueIfError);
        }
    }
#endif

    void clearStreamProperties(); ///< Clear all array of stream properties

private:
    const FormatContext* _formatContext;     ///< Has link (no ownership)
    const AVFormatContext* _avFormatContext; ///< Has link (no ownership)

    std::map<size_t, StreamProperties*>
        _streams; ///< Map of properties per stream index (of all types) - only references to the following properties

    std::vector<VideoProperties> _videoStreams;             ///< Array of properties per video stream
    std::vector<AudioProperties> _audioStreams;             ///< Array of properties per audio stream
    std::vector<DataProperties> _dataStreams;               ///< Array of properties per data stream
    std::vector<SubtitleProperties> _subtitleStreams;       ///< Array of properties per subtitle stream
    std::vector<AttachementProperties> _attachementStreams; ///< Array of properties per attachement stream
    std::vector<UnknownProperties> _unknownStreams;         ///< Array of properties per unknown stream

    PropertyVector _metadatas;
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const FileProperties& fileProperties);
#endif
}

#endif
