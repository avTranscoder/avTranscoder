#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_DATA_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_DATA_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>

extern "C" {
#include <libavformat/avformat.h>
}

namespace avtranscoder
{

class AvExport DataProperties
{
public:
	DataProperties( const AVFormatContext* formatContext, const size_t index );

	size_t getStreamId() const { return _streamId; }
	PropertiesMap& getMetadatas() { return _metadatas; }

#ifndef SWIG
	const AVFormatContext& getAVFormatContext() { return *_formatContext; }
#endif

	PropertiesMap getPropertiesAsMap() const;  ///< Return all data properties as a map (name of property: value)

private:
	void detectAncillaryData();

private:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)

	size_t _streamId;
	PropertiesMap _metadatas;
};

}

#endif
