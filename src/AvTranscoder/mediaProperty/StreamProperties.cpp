#include "StreamProperties.hpp"

#include <AvTranscoder/JsonWriter.hpp>

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

Rational StreamProperties::getTimeBase() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->streams[_streamIndex]->time_base;
}

float StreamProperties::getDuration() const
{
	const Rational timeBase = getTimeBase();
	return av_q2d( timeBase ) * _formatContext->streams[_streamIndex]->duration;
}

AVMediaType StreamProperties::getStreamType() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->streams[_streamIndex]->codec->codec_type;
}

PropertyVector StreamProperties::getPropertiesAsVector() const
{
	PropertyVector data;

	addProperty( data, "streamId", &StreamProperties::getStreamId );
	addProperty( data, "streamIndex", &StreamProperties::getStreamIndex );
	addProperty( data, "timeBase", &StreamProperties::getTimeBase );
	addProperty( data, "duration", &StreamProperties::getDuration );

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

std::string StreamProperties::getPropertiesAsJson() const
{
	json::JsonObjectStreamWriter writer;
	PropertyMap properties = getPropertiesAsMap();
	for(PropertyMap::iterator it = properties.begin(); it != properties.end(); ++it)
		writer << std::make_pair(it->first.c_str(), it->second.c_str());
	return writer.build();
}

}
