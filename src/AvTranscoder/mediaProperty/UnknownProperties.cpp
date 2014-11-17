#include "UnknownProperties.hpp"

namespace avtranscoder
{

UnknownProperties::UnknownProperties( const AVFormatContext* formatContext, const size_t index )
	: _formatContext( formatContext )
	, _streamId( index )
{}

MetadatasMap UnknownProperties::getDataMap() const
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
