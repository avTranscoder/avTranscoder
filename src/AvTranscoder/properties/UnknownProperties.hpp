#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_UNKNOWN_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_UNKNOWN_PROPERTIES_HPP

#include <AvTranscoder/properties/StreamProperties.hpp>

namespace avtranscoder
{

class AvExport UnknownProperties : public StreamProperties
{
public:
    UnknownProperties(const FileProperties& fileProperties, const size_t index)
        : StreamProperties(fileProperties, index)
    {
    }
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const UnknownProperties& unknownProperties);
#endif
}

#endif
