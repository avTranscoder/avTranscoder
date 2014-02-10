#include "OutputFile.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/avstring.h>
}

#include <iostream>

namespace avtranscoder
{

OutputFile::OutputFile( const std::string& file )
	: outputFormat  ( NULL )
	, formatContext ( NULL )
	, codec         ( NULL )
	, codecContext  ( NULL )
	, stream        ( NULL )
	, filename      ( file )
	, packetCount   ( 0 )
{
	av_register_all();  // Warning: should be called only once
}

bool OutputFile::setup()
{
	outputFormat = av_guess_format( NULL, filename.c_str(), NULL);

	if( !outputFormat )
		return false;

	if( ( formatContext = avformat_alloc_context() ) == NULL )
	{
		return false;
	}

	formatContext->oformat = outputFormat;

	if( !( outputFormat->flags & AVFMT_NOFILE ) )
	{
		if( avio_open2( &formatContext->pb, filename.c_str(), AVIO_FLAG_WRITE, NULL, NULL ) < 0 )
		{
			// @TODO free format here
			return false;
		}
	}

	return formatContext != NULL;
}

bool OutputFile::addVideoStream( const VideoDesc& videoDesc )
{
	if( formatContext == NULL )
		return false; // throw not return ...

	if( ( stream = avformat_new_stream( formatContext, videoDesc.getCodec() ) ) == NULL )
		return false;

	// need to set the time_base on the AVCodecContext and the AVStream...
	stream->codec->time_base = videoDesc.getCodecContext()->time_base;
	stream->time_base = stream->codec->time_base;

	stream->codec->width  = videoDesc.getCodecContext()->width;
	stream->codec->height = videoDesc.getCodecContext()->height;
	stream->codec->bit_rate = 50000000;
	stream->codec->time_base.num = 1;
	stream->codec->time_base.den = 25;
	stream->codec->pix_fmt = AV_PIX_FMT_YUV422P;

	// to move in endSetup
	if( avformat_write_header( formatContext, NULL ) != 0 )
	{
		std::cout << "could not write header" << std::endl;
		return false;
	}

	return true;
}

bool OutputFile::addAudioStream( )
{
	if( formatContext == NULL )
		return false;

	av_register_all();  // Warning: should be called only once
	AVCodec*  codec  = NULL;
	AVStream* stream = NULL;

	if( ( codec = avcodec_find_encoder_by_name( "wav" ) ) == NULL )
		return false;

	if( ( stream = avformat_new_stream( formatContext, codec ) ) == NULL )
		return false;

	//stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	return true;
}

bool OutputFile::wrap( const Image& data, const size_t streamId )
{
	AVPacket packet;
	av_init_packet( &packet );

	packet.size = data.getSize();
	packet.data = (uint8_t*)data.getPtr();
	packet.stream_index = streamId;

	packet.dts = 0;
	packet.pts = packetCount;

	if( av_interleaved_write_frame( formatContext, &packet ) != 0 )
	{
		std::cout << "error when writting packet in stream" << std::endl;
		return false;
	}

	av_free_packet( &packet );

	packetCount++;
	return true;
}

bool OutputFile::wrap( const DataStream& data, const size_t streamId )
{
	AVPacket packet;
	av_init_packet( &packet );

	packet.size = data.getSize();
	packet.data = (uint8_t*)data.getPtr();
	packet.stream_index = streamId;

	packet.dts = 0;
	packet.pts = packetCount;

	if( av_interleaved_write_frame( formatContext, &packet ) != 0 )
	{
		std::cout << "error when writting packet in stream" << std::endl;
		return false;
	}

	av_free_packet( &packet );

	packetCount++;
	return true;
}

}
