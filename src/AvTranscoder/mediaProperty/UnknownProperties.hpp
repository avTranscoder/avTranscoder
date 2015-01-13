#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_UNKNOWN_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_UNKNOWN_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>

namespace avtranscoder
{

class AvExport UnknownProperties
{
public:
	UnknownProperties( const FormatContext& formatContext, const size_t index );

	size_t getStreamId() const { return _streamId; }
	PropertiesMap& getMetadatas() { return _metadatas; }

#ifndef SWIG
	const AVFormatContext& getAVFormatContext() { return *_formatContext; }
#endif

	PropertiesMap getPropertiesAsMap() const;  ///< Return unknown properties as a map (name of property: value)

private:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)

	size_t _streamId;
	PropertiesMap _metadatas;
};

}

#endif
