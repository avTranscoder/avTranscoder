#include "AttachementProperties.hpp"

namespace avtranscoder
{


AttachementProperties::AttachementProperties()
	: _formatContext( NULL )
	, _streamId( 0 )
{}

AttachementProperties::AttachementProperties( const AVFormatContext* formatContext, const size_t index )
	: _formatContext( formatContext )
	, _streamId( index )
{}

MetadatasMap AttachementProperties::getDataMap() const
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
