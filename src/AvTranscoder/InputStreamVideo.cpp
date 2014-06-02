#include "InputStreamVideo.hpp"
#include "common.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include "AvInputStream.hpp"

#include <iostream>
#include <stdexcept>

namespace avtranscoder
{

InputStreamVideo::InputStreamVideo( AvInputStream& inputStream )
	: m_inputStream   ( &inputStream )
	, m_codec         ( NULL )
	, m_codecContext  ( NULL )
	, m_frame         ( NULL )
	, m_selectedStream( -1 )
{
	av_register_all();

	m_codec = avcodec_find_decoder( m_inputStream->getVideoDesc().getVideoCodecId() );
	if( m_codec == NULL )
	{
		throw std::runtime_error( "codec not supported" );
	}

	m_codecContext = avcodec_alloc_context3( m_codec );
	if( m_codecContext == NULL )
	{
		throw std::runtime_error( "unable to find context for codec" );
	}

	// if( codec->capabilities & CODEC_CAP_TRUNCATED )
	// 	codecContext->flags |= CODEC_FLAG_TRUNCATED;

	int ret = avcodec_open2( m_codecContext, m_codec, NULL );

	if( ret < 0 || m_codecContext == NULL || m_codec == NULL )
	{
		std::string msg = "unable open video codec: ";
		msg +=  m_codec->long_name;
		msg += " (";
		msg += m_codec->name;
		msg += ")";
		avcodec_close( m_codecContext );
		throw std::runtime_error( msg );
	}

#if LIBAVCODEC_VERSION_MAJOR > 54
	m_frame = av_frame_alloc();
#else
	m_frame = avcodec_alloc_frame();
#endif
	if( m_frame == NULL )
	{
		throw std::runtime_error( "unable to setup frame buffer" );
	}
}

InputStreamVideo::~InputStreamVideo()
{
	if( m_codecContext != NULL )
	{
		//avcodec_close( m_codecContext );
		av_free( m_codecContext );
		m_codecContext = NULL;
	}
	if( m_frame != NULL )
	{
#if LIBAVCODEC_VERSION_MAJOR > 54
		av_frame_free( &m_frame );
#else
 #if LIBAVCODEC_VERSION_MAJOR > 53
		avcodec_free_frame( &m_frame );
 #else
		av_free( m_frame );
 #endif
#endif
		m_frame = NULL;
	}
}

bool InputStreamVideo::readNextFrame( Image& frameBuffer )
{
	int got_frame = 0;

	while( ! got_frame )
	{
		DataStream data;
		if( ! m_inputStream->readNextPacket( data ) )
			return false;

		AVPacket packet;
		av_init_packet( &packet );

		packet.stream_index = m_selectedStream;
		packet.data         = data.getPtr();
		packet.size         = data.getSize();

		avcodec_decode_video2( m_codecContext, m_frame, &got_frame, &packet );

		av_free_packet( &packet );
	}

	size_t decodedSize = avpicture_get_size( (AVPixelFormat)m_frame->format, m_frame->width, m_frame->height );
	if( frameBuffer.getBuffer().size() != decodedSize )
		frameBuffer.getBuffer().resize( avpicture_get_size( (AVPixelFormat)m_frame->format, m_frame->width, m_frame->height ) );

	// Copy pixel data from an AVPicture into one contiguous buffer.
	avpicture_layout( (AVPicture*)m_frame, (AVPixelFormat)m_frame->format, m_frame->width, m_frame->height, &frameBuffer.getBuffer()[0], frameBuffer.getBuffer().size() );

	return true;
}

void InputStreamVideo::flushDecoder()
{
	avcodec_flush_buffers( m_codecContext );
}

}