#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_ATTACHEMENT_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_ATTACHEMENT_PROPERTIES_HPP

#include <AvTranscoder/properties/StreamProperties.hpp>

namespace avtranscoder
{

class AvExport AttachementProperties : public StreamProperties
{
public:
    AttachementProperties(const FormatContext& formatContext, const size_t index)
        : StreamProperties(formatContext, index)
    {
    }
};
}

#endif
