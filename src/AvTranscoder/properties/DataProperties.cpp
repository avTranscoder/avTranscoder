#include "DataProperties.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <bitset>

namespace avtranscoder
{

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
