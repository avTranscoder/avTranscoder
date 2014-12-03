#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_ATTACHEMENT_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_ATTACHEMENT_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>

extern "C" {
#include <libavformat/avformat.h>
}

namespace avtranscoder
{

class AvExport AttachementProperties
{
public:
	AttachementProperties( const AVFormatContext* formatContext, const size_t index );

	size_t getStreamId() const { return _streamId; }
	PropertiesMap& getMetadatas() { return _metadatas; }

#ifndef SWIG
	const AVFormatContext& getAVFormatContext() { return *_formatContext; }
#endif

	PropertiesMap getDataMap() const;

private:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)

	size_t _streamId;
	PropertiesMap _metadatas;
};

}

#endif
