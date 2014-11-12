#include "SubtitleProperties.hpp"

namespace avtranscoder
{

SubtitleProperties::SubtitleProperties()
	: _formatContext( NULL )
	, _streamId( 0 )
{}

SubtitleProperties::SubtitleProperties( const AVFormatContext* formatContext, const size_t index )
	: _formatContext( formatContext )
	, _streamId( index )
{}

MetadatasMap SubtitleProperties::getDataMap() const
{
	MetadatasMap dataMap;

	detail::add( dataMap, "streamId", _streamId );

	for( size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, _metadatas.at( metadataIndex ).first, _metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

}
