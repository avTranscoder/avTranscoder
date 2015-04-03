#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_DATA_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_DATA_PROPERTIES_HPP

#include <AvTranscoder/mediaProperty/StreamProperties.hpp>

namespace avtranscoder
{

class AvExport DataProperties : public StreamProperties
{
public:
	DataProperties( const FormatContext& formatContext, const size_t index )
	    : StreamProperties( formatContext, index )
	{}

private:
	void detectAncillaryData();
};

}

#endif
