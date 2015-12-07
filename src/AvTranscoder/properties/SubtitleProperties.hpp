#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_SUBTITLE_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_SUBTITLE_PROPERTIES_HPP

#include <AvTranscoder/properties/StreamProperties.hpp>

namespace avtranscoder
{

class AvExport SubtitleProperties : public StreamProperties
{
public:
	SubtitleProperties( const FormatContext& formatContext, const size_t index )
	    : StreamProperties( formatContext, index )
	{}
};

}

#endif
