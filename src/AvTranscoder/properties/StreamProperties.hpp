#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_STREAM_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_STREAM_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/Option.hpp>
#include <AvTranscoder/properties/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>

namespace avtranscoder
{

class FileProperties;

/// Virtual based class of properties for all types of stream
class AvExport StreamProperties
{
public:
    StreamProperties(const FileProperties& fileProperties, const size_t index);
    virtual ~StreamProperties() = 0;

    size_t getStreamIndex() const { return _streamIndex; }
    size_t getStreamId() const;
    Rational getTimeBase() const;
    float getDuration() const; ///< in seconds, 0 if not available
    AVMediaType getStreamType() const;

    size_t getCodecId() const;
    std::string getCodecName() const;
    std::string getCodecLongName() const;

    const PropertyVector& getMetadatas() const { return _metadatas; }

    /**
     * @return The list of private codec options.
     * @see getOptionsMap
     * @see getOption
     */
    OptionArray getCodecOptions();
#ifndef SWIG
    OptionMap& getCodecOptionsMap() { return _options; } ///< Get options as map
#endif
    Option& getCodecOption(const std::string& optionName) { return _options.at(optionName); }

#ifndef SWIG
    const AVFormatContext& getAVFormatContext() const { return *_formatContext; }
#endif

    std::string asJson() const;      ///< Return all properties as a json format.
    PropertyMap asMap() const;       ///< Return all properties as a map (name of property, value)
    PropertyVector asVector() const; ///< Same data with a specific order
    virtual PropertyVector& fillVector(PropertyVector& data) const; ///< To avoid copy of the vector

private:
#ifndef SWIG
    template <typename T>
    void addProperty(PropertyVector& dataVector, const std::string& key, T (StreamProperties::*getter)(void) const) const
    {
        try
        {
            detail::add(dataVector, key, (this->*getter)());
        }
        catch(const std::exception& e)
        {
            detail::add(dataVector, key, detail::propertyValueIfError);
        }
    }
#endif

protected:
    const FileProperties* _fileProperties; ///< Has link (no ownership)
    const AVFormatContext* _formatContext; ///< Has link (no ownership)
    AVCodecContext* _codecContext;         ///< Has link (no ownership)
    AVCodec* _codec;                       ///< Has link (no ownership)

    size_t _streamIndex;
    PropertyVector _metadatas;

    OptionMap _options;
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const StreamProperties& streamProperties);
#endif
}

#endif
