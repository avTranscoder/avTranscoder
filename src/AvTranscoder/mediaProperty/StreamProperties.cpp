#include "StreamProperties.hpp"

#include <stdexcept>

namespace avtranscoder
{

StreamProperties::StreamProperties( const FormatContext& formatContext, const size_t index )
	: _formatContext( &formatContext.getAVFormatContext() )
	, _streamIndex( index )
{
	if( _formatContext )
		detail::fillMetadataDictionnary( _formatContext->streams[index]->metadata, _metadatas );
}

StreamProperties::~StreamProperties()
{
	
}

size_t StreamProperties::getStreamId() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->streams[_streamIndex]->id;
}

PropertyVector StreamProperties::getPropertiesAsVector() const
{
	PropertyVector data;

	try
	{
		detail::add( data, "streamId", getStreamId() );
	}
	catch( const std::exception& e )
	{
		detail::add( data, "streamId", e.what() );
	}

	for( size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex )
	{
		detail::add( data, _metadatas.at( metadataIndex ).first, _metadatas.at( metadataIndex ).second );
	}

	return data;
}

PropertyMap StreamProperties::getPropertiesAsMap() const
{
	PropertyMap dataMap;

	PropertyVector dataVector( getPropertiesAsVector() );
	for( PropertyVector::const_iterator it = dataVector.begin();
			it != dataVector.end();
			++it )
	{
		dataMap.insert( std::make_pair( it->first, it->second ) );
	}

	return dataMap;
}

}
