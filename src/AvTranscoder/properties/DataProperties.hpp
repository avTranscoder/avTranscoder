#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_DATA_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_DATA_PROPERTIES_HPP

#include <AvTranscoder/properties/StreamProperties.hpp>

namespace avtranscoder
{

class AvExport DataProperties : public StreamProperties
{
public:
    DataProperties(const FileProperties& fileProperties, const size_t index)
        : StreamProperties(fileProperties, index)
    {
    }

private:
    void detectAncillaryData();
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const DataProperties& dataProperties);
#endif
}

#endif
