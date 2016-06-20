#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_SUBTITLE_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_SUBTITLE_PROPERTIES_HPP

#include <AvTranscoder/properties/StreamProperties.hpp>

namespace avtranscoder
{

class AvExport SubtitleProperties : public StreamProperties
{
public:
    SubtitleProperties(const FileProperties& fileProperties, const size_t index)
        : StreamProperties(fileProperties, index)
    {
    }
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const SubtitleProperties& subtitleProperties);
#endif
}

#endif
