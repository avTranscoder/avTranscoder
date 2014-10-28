#ifndef _MEDIA_MANAGER_IO_DATA_STREAM_PROPERTIES_HPP_
#define _MEDIA_MANAGER_IO_DATA_STREAM_PROPERTIES_HPP_

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <bitset>

namespace mediamanager
{
namespace mediaio
{


void detectAncillaryData( AVFormatContext* formatContext, const int index )
{
	AVPacket pkt;
	av_init_packet( &pkt );
	
	bool detection = false;

	while( ! av_read_frame( formatContext, &pkt ) )
	{
		if( pkt.stream_index == index )
		{
			std::cout << "start detect packet" << std::endl;
			size_t offset = 0;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;
			std::cout << offset << " - " << (int) pkt.data[ offset ] << " | " << std::bitset<8>( pkt.data[ offset ] ) << std::endl; offset++;


			unsigned short numberOfLines = (unsigned int) ( pkt.data[0] << 8 ) + pkt.data[1];

			std::cout << "[data] number of lines " << numberOfLines << std::endl;

			detection = true;
		}

		av_free_packet( &pkt );

		if( detection )
			break;
	}
}

DataProperties dataStreamInfo( AVFormatContext* formatContext, const size_t index )
{
	DataProperties dp;
	dp.streamId = index;

	// AVCodecContext* codec_context = formatContext->streams[index]->codec;
	
	// dp.codecId       = codec_context->codec_id;

	//detectAncillaryData( formatContext, index );

	return dp;
}

}
}

#endif