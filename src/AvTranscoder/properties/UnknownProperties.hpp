#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_UNKNOWN_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_UNKNOWN_PROPERTIES_HPP

#include <AvTranscoder/properties/StreamProperties.hpp>

namespace avtranscoder
{

class AvExport UnknownProperties : public StreamProperties
{
public:
    UnknownProperties(const FormatContext& formatContext, const size_t index)
        : StreamProperties(formatContext, index)
    {
    }
};
}

#endif
