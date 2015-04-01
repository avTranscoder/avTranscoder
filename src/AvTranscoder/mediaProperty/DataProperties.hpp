#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_DATA_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_DATA_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>

namespace avtranscoder
{

class AvExport DataProperties
{
public:
	DataProperties( const FormatContext& formatContext, const size_t index );

	size_t getStreamIndex() const { return _streamIndex; }
	size_t getStreamId() const;
	PropertyVector& getMetadatas() { return _metadatas; }

#ifndef SWIG
	const AVFormatContext& getAVFormatContext() { return *_formatContext; }
#endif

	PropertyVector getPropertiesAsVector() const;  ///< Return all data properties as a vector (name of property: value)

private:
	void detectAncillaryData();

private:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)

	size_t _streamIndex;
	PropertyVector _metadatas;
};

}

#endif
