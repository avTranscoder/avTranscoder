#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_STREAM_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_STREAM_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/properties/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>

namespace avtranscoder
{

/// Virtual based class of properties for all types of stream
class AvExport StreamProperties
{
public:
    StreamProperties(const FormatContext& formatContext, const size_t index);
    virtual ~StreamProperties() = 0;

    size_t getStreamIndex() const { return _streamIndex; }
    size_t getStreamId() const;
    Rational getTimeBase() const;
    float getDuration() const; ///< in seconds
    AVMediaType getStreamType() const;

    size_t getCodecId() const;
    std::string getCodecName() const;
    std::string getCodecLongName() const;

    const PropertyVector& getMetadatas() const { return _metadatas; }

#ifndef SWIG
    const AVFormatContext& getAVFormatContext() const { return *_formatContext; }
#endif

    std::string asJson() const;              ///< Return all properties as a json format.
    PropertyMap asMap() const;               ///< Return all properties as a map (name of property, value)
    virtual PropertyVector asVector() const; ///< Same data with a specific order

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
    const AVFormatContext* _formatContext; ///< Has link (no ownership)
    AVCodecContext* _codecContext;         ///< Has link (no ownership)
    AVCodec* _codec;                       ///< Has link (no ownership)

    size_t _streamIndex;
    PropertyVector _metadatas;
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const StreamProperties& streamProperties);
#endif
}

#endif
