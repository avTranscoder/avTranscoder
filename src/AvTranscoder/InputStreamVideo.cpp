#include "InputStreamVideo.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <iostream>

namespace avtranscoder
{

InputStreamVideo::InputStreamVideo()
	: formatContext( NULL )
	, codec        ( NULL )
	, codecContext ( NULL )
	, selectedStream( -1 )
{}

bool InputStreamVideo::setup( const std::string& filename, const size_t streamIndex )
{
	av_register_all();

	if( avformat_open_input( &formatContext, filename.c_str(), NULL, NULL ) < 0 )
	{
		return false;
	}

	// update format context informations from streams
	if( avformat_find_stream_info( formatContext, NULL ) < 0 )
	{
		return false;
	}

	size_t videoStreamCount = 0;
	for( size_t streamId = 0; streamId < formatContext->nb_streams; streamId++ )
	{
		if( formatContext->streams[streamId]->codec->codec_type == AVMEDIA_TYPE_VIDEO )
		{
			if( videoStreamCount == streamIndex )
			{
				selectedStream = streamId;
			}
			videoStreamCount++;
		}
	}

	if( selectedStream == -1 )
		return false;

	codec = avcodec_find_decoder( formatContext->streams[selectedStream]->codec->codec_id );
	if( codec == NULL )
	{
		return false;
	}

	codecContext = avcodec_alloc_context3( codec );

	if( codecContext == NULL )
	{
		return false;
	}

	// if( codec->capabilities & CODEC_CAP_TRUNCATED )
	// 	codecContext->flags|= CODEC_FLAG_TRUNCATED;

	avcodec_open2( codecContext, codec, NULL );

	if( codecContext == NULL || codec == NULL )
	{
		return false;
	}

	return true;
}

Image& InputStreamVideo::readNextFrame( Image& frameBuffer )
{
	AVPacket pkt;
	AVFrame* frame = av_frame_alloc();

	int got_frame = 0;

	while( ! got_frame )
	{
		av_init_packet( &pkt );
		int ret = av_read_frame( formatContext, &pkt );
		if( ret < 0 ) // error or end of file
			return frameBuffer;

		// We only read one stream and skip others
		if( pkt.stream_index == selectedStream )
		{
			avcodec_decode_video2( codecContext, frame, &got_frame, &pkt );
		}
		av_free_packet( &pkt );
	}

	frameBuffer.getBuffer().resize( avpicture_get_size( (AVPixelFormat)frame->format, frame->width, frame->height ) );

	// Copy pixel data from an AVPicture into one contiguous buffer.
	avpicture_layout( (AVPicture*)frame, (AVPixelFormat)frame->format, frame->width, frame->height, &frameBuffer.getBuffer()[0], frameBuffer.getBuffer().size() );

	//av_frame_free( &frame ); // desallocate all memory ...

	return frameBuffer;
}

}