#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_ATTACHEMENT_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_ATTACHEMENT_PROPERTIES_HPP

#include <AvTranscoder/properties/StreamProperties.hpp>

namespace avtranscoder
{

class AvExport AttachementProperties : public StreamProperties
{
public:
    AttachementProperties(const FileProperties& fileProperties, const size_t index)
        : StreamProperties(fileProperties, index)
    {
    }
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const AttachementProperties& attachementProperties);
#endif
}

#endif
