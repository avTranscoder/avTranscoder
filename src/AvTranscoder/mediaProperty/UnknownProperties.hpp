#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_UNKNOWN_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_UNKNOWN_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/mediaProperty.hpp>

extern "C" {
#include <libavformat/avformat.h>
}

namespace avtranscoder
{

class AvExport UnknownProperties
{
public:
	UnknownProperties();
	UnknownProperties( const AVFormatContext* formatContext, const size_t index );

	size_t getStreamId() const { return _streamId; }
	MetadatasMap& getMetadatas() { return _metadatas; }

	const AVFormatContext& getAVFormatContext() { return *_formatContext; }

	MetadatasMap getDataMap() const;

private:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)

	size_t _streamId;
	MetadatasMap _metadatas;
};

}

#endif
