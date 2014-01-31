#include "OutputStreamVideo.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
}

#include <iostream>

namespace avtranscoder
{

OutputStreamVideo::OutputStreamVideo()
	: codec ( NULL )
	, codecContext( NULL )
	, width ( 0 )
	, height( 0 )
	, pixelFormat( AV_PIX_FMT_YUV422P )
{
}

bool OutputStreamVideo::setup()
{
	av_register_all();  // Warning: should be called only once

	// av_log_set_level( AV_LOG_DEBUG );

	if( ( codec = avcodec_find_encoder_by_name( "dnxhd" ) ) == NULL )
		return false;

	if( ( codecContext = avcodec_alloc_context3( codec ) ) == NULL )
		return false;

	// Set default codec parameters
	if( avcodec_get_context_defaults3( codecContext, codec ) != 0 )
		return false;

	// set codec parameters
	codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
	codecContext->width  = width;
	codecContext->height = height;
	codecContext->bit_rate = 120000000;
	codecContext->time_base.num = 1;
	codecContext->time_base.den = 25;
	codecContext->pix_fmt = pixelFormat;

	// try to open encoder with parameters.
	if( avcodec_open2( codecContext, codec, NULL ) < 0 )
		return false;

	return true;
}


bool OutputStreamVideo::encodeFrame( const std::vector<unsigned char>& sourceImage, std::vector<unsigned char>& codedFrame )
{
	AVFrame* frame = avcodec_alloc_frame();

	// Set default frame parameters
	avcodec_get_frame_defaults( frame );

	frame->width  = width;
	frame->height = height;
	frame->format = pixelFormat;
	avpicture_fill( (AVPicture*)frame, const_cast<unsigned char *>( &sourceImage[0] ), pixelFormat, width, height );

	AVPacket packet;
	av_init_packet( &packet );
	// avcodec_encode_video allocate packet
	packet.size = 0;
	packet.data = NULL;
	packet.stream_index = 0;

	if( ( codecContext->coded_frame ) &&
		( codecContext->coded_frame->pts != (int)AV_NOPTS_VALUE ) )
	{
		// why need to do that ?
		packet.pts = av_rescale_q( codecContext->coded_frame->pts, codecContext->time_base, codecContext->time_base );

		std::cout << "pts with rescale " << packet.pts << std::endl;
		packet.pts = codecContext->coded_frame->pts;
		std::cout << "pts without rescale " << packet.pts << std::endl;
	}

	if( codecContext->coded_frame &&
		codecContext->coded_frame->key_frame )
	{
		packet.flags |= AV_PKT_FLAG_KEY;
	}

	int gotPacket = 0;
	int ret = avcodec_encode_video2( codecContext, &packet, frame, &gotPacket );

	if( ret == 0 && gotPacket == 1 )
	{
		codedFrame.resize( packet.size );
		memcpy( &codedFrame[0], packet.data, packet.size );
	}

	av_frame_free( &frame );
	return ret < 0;
}

}