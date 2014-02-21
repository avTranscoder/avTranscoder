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

OutputStreamVideo::OutputStreamVideo( )
	: m_videoDesc( "mpeg2video" )
{
}

bool OutputStreamVideo::setup( )
{
	av_register_all();  // Warning: should be called only once

	AVCodecContext* codecContext;
	codecContext = m_videoDesc.getCodecContext();

	if( codecContext == NULL )
		return false;

	// try to open encoder with parameters.
	if( avcodec_open2( m_videoDesc.getCodecContext(), m_videoDesc.getCodec(), NULL ) < 0 )
		return false;

	return true;
}


bool OutputStreamVideo::encodeFrame( const Image& sourceImage, DataStream& codedFrame )
{
	AVFrame* frame = avcodec_alloc_frame();

	AVCodecContext* codecContext = m_videoDesc.getCodecContext();

	// Set default frame parameters
	avcodec_get_frame_defaults( frame );

	frame->width  = codecContext->width;
	frame->height = codecContext->height;
	frame->format = codecContext->pix_fmt;
	avpicture_fill( (AVPicture*)frame, const_cast< unsigned char * >( sourceImage.getPtr() ), codecContext->pix_fmt, codecContext->width, codecContext->height );

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
		//packet.pts = av_rescale_q( codecContext->coded_frame->pts, codecContext->time_base, codecContext->time_base );

		//std::cout << "pts with rescale " << (int)packet.pts << std::endl;
		packet.pts = codecContext->coded_frame->pts;
		//std::cout << "pts without rescale " << (int)packet.pts << std::endl;
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
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}

	av_free_packet( &packet );
	av_frame_free( &frame );
	return ret < 0;
}

}