#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_STREAM_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_STREAM_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>

namespace avtranscoder
{

/// Virtual based class of properties for all types of stream
class AvExport StreamProperties
{
public:
	StreamProperties( const FormatContext& formatContext, const size_t index );
	virtual ~StreamProperties() = 0;

	size_t getStreamIndex() const { return _streamIndex; }
	size_t getStreamId() const;
	PropertyVector& getMetadatas() { return _metadatas; }

#ifndef SWIG
	const AVFormatContext& getAVFormatContext() { return *_formatContext; }
#endif

	PropertyMap getPropertiesAsMap() const;  ///< Return all properties as a map (name of property, value)
	PropertyVector getPropertiesAsVector() const;  ///< Same data with a specific order

protected:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)

	size_t _streamIndex;
	PropertyVector _metadatas;
};

}

#endif
