#include "DataProperties.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <bitset>
#include <stdexcept>

namespace avtranscoder
{

DataProperties::DataProperties( const FormatContext& formatContext, const size_t index )
	: _formatContext( &formatContext.getAVFormatContext() )
	, _streamIndex( index )
{
	//detectAncillaryData( _formatContext, _streamIndex );

	if( _formatContext )
		detail::fillMetadataDictionnary( _formatContext->streams[index]->metadata, _metadatas );
}

size_t DataProperties::getStreamId() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->streams[_streamIndex]->id;
}

PropertiesMap DataProperties::getPropertiesAsMap() const
{
	PropertiesMap dataMap;

	try{ detail::add( dataMap, "streamId", getStreamId() ); } catch(std::exception& e){ detail::add( dataMap, "streamId", e.what() ); }

	for( size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, _metadatas.at( metadataIndex ).first, _metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

void DataProperties::detectAncillaryData()
{
	AVPacket pkt;
	av_init_packet( &pkt );
	
	bool detection = false;

	while( ! av_read_frame( const_cast<AVFormatContext*>( _formatContext ), &pkt ) )
	{
		if( pkt.stream_index == (int)_streamIndex )
		{
			LOG_DEBUG( "DataProperties - start detect packet" );

			size_t offset = 0;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;
			LOG_DEBUG( offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) )
			offset++;

			unsigned short numberOfLines = (unsigned int) ( pkt.data[0] << 8 ) + pkt.data[1];

			LOG_DEBUG( "DataProperties - number of lines " << numberOfLines )

			detection = true;
		}

		av_free_packet( &pkt );

		if( detection )
			break;
	}
}

}
