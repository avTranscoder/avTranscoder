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

#include "Profile.hpp"

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

	AVCodecContext* codecContext( m_videoDesc.getCodecContext() );

	if( codecContext == NULL )
		return false;

	// try to open encoder with parameters.
	if( avcodec_open2( m_videoDesc.getCodecContext(), m_videoDesc.getCodec(), NULL ) < 0 )
		return false;

	return true;
}


bool OutputStreamVideo::encodeFrame( const Image& sourceImage, DataStream& codedFrame )
{
#if LIBAVCODEC_VERSION_MAJOR > 54
	AVFrame* frame = av_frame_alloc();
#else
	AVFrame* frame = avcodec_alloc_frame();
#endif

	AVCodecContext* codecContext = m_videoDesc.getCodecContext();

	// Set default frame parameters
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_unref( frame );
#else
	avcodec_get_frame_defaults( frame );
#endif

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

#if LIBAVCODEC_VERSION_MAJOR > 53
	int gotPacket = 0;
	int ret = avcodec_encode_video2( codecContext, &packet, frame, &gotPacket );
	if( ret == 0 && gotPacket == 1 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
#else
	int ret = avcodec_encode_video( codecContext, packet.data, packet.size, frame );
	if( ret > 0 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
#endif
/*
		std::string imgType = "";
		switch( codecContext->coded_frame->pict_type )
		{
			case AV_PICTURE_TYPE_NONE : imgType = "None"; break;
			case AV_PICTURE_TYPE_I : imgType = "I"; break;
			case AV_PICTURE_TYPE_P : imgType = "P"; break;
			case AV_PICTURE_TYPE_B : imgType = "B"; break;
			case AV_PICTURE_TYPE_S : imgType = "S"; break;
			case AV_PICTURE_TYPE_SI : imgType = "SI"; break;
			case AV_PICTURE_TYPE_SP : imgType = "SP"; break;
			case AV_PICTURE_TYPE_BI : imgType = "BI"; break;
		}

		std::clog << "\tframe " << codecContext->coded_frame->display_picture_number;
		std::clog << " coded @ " << codecContext->coded_frame->coded_picture_number;
		std::clog << " type : " << imgType;
		std::clog << " quality : " << codecContext->coded_frame->quality << std::endl;
*/
	av_free_packet( &packet );
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_free( &frame );
	return ret == 0 && gotPacket == 1;
#else
 #if LIBAVCODEC_VERSION_MAJOR > 53
	avcodec_free_frame( &frame );
	return ret == 0 && gotPacket == 1;
 #else
	av_free( frame );
 #endif
#endif
	return ret == 0;
}


bool OutputStreamVideo::encodeFrame( DataStream& codedFrame )
{
	AVCodecContext* codecContext = m_videoDesc.getCodecContext();

	AVPacket packet;
	av_init_packet( &packet );
	// avcodec_encode_video allocate packet
	packet.size = 0;
	packet.data = NULL;
	packet.stream_index = 0;

#if LIBAVCODEC_VERSION_MAJOR > 53
	int gotPacket = 0;
	int ret = avcodec_encode_video2( codecContext, &packet, NULL, &gotPacket );
	if( ret == 0 && gotPacket == 1 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
	av_free_packet( &packet );
	return ret == 0 && gotPacket == 1;

#else
	int ret = avcodec_encode_video( codecContext, packet.data, packet.size, NULL );
	if( ret > 0 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
	av_free_packet( &packet );
	return ret == 0;

#endif
}

void OutputStreamVideo::setProfile( const std::string& profile )
{
	Profile p;
	p.loadProfiles();

	Profile::ProfileDesc prof = p.getProfile( profile );

	m_videoDesc.setVideoCodec( prof["codec"] );
	m_videoDesc.setTimeBase( 1, 25 ); // 25 fps
	m_videoDesc.setImageParameters( 1920, 1080, av_get_pix_fmt( prof["pix_fmt"].c_str() ) );

	m_videoDesc.set( "b", prof["b"] );

	setup();

	for( Profile::ProfileDesc::iterator it = prof.begin(); it != prof.end(); ++it )
	{
		if( (*it).first == "avProfile" )
			continue;
		if( (*it).first == "avProfileLong" )
			continue;
		if( (*it).first == "codec" )
			continue;
		if( (*it).first == "pix_fmt" )
			continue;
		if( (*it).first == "width" )
			continue;
		if( (*it).first == "height" )
			continue;

		m_videoDesc.set( (*it).first, (*it).second );
	}
}

}