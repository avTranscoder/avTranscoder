#include "SubtitleProperties.hpp"

#include <stdexcept>

namespace avtranscoder
{

SubtitleProperties::SubtitleProperties( const FormatContext& formatContext, const size_t index )
	: _formatContext( &formatContext.getAVFormatContext() )
	, _streamIndex( index )
{
	if( _formatContext )
		detail::fillMetadataDictionnary( _formatContext->streams[index]->metadata, _metadatas );
}

size_t SubtitleProperties::getStreamId() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->streams[_streamIndex]->id;
}

PropertyVector SubtitleProperties::getPropertiesAsVector() const
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

}
