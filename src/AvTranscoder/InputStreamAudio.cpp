#include "InputStreamAudio.hpp"
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

#include <iostream>
#include <stdexcept>

namespace avtranscoder
{

InputStreamAudio::InputStreamAudio( const InputStream& inputStream )
	: m_inputStream   ( inputStream )
	, m_codec         ( NULL )
	, m_codecContext  ( NULL )
	, m_frame         ( NULL )
	, m_selectedStream( -1 )
{
	av_register_all();

	m_codec = avcodec_find_decoder( m_inputStream.getAudioDesc().getAudioCodecId() );
	if( m_codec == NULL )
	{
		throw std::runtime_error( "codec not supported" );
	}

	m_codecContext = avcodec_alloc_context3( m_codec );
	if( m_codecContext == NULL )
	{
		throw std::runtime_error( "unable to find context for codec" );
	}

	avcodec_open2( m_codecContext, m_codec, NULL );
	if( m_codecContext == NULL || m_codec == NULL )
	{
		throw std::runtime_error( "unable open codec" );
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

InputStreamAudio::~InputStreamAudio()
{
	if( m_codecContext != NULL )
	{
		avcodec_close( m_codecContext );
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

bool InputStreamAudio::readNextFrame( AudioFrame& audioFrameBuffer )
{
/*	int got_frame = 0;

	while( ! got_frame )
	{
		AVPacket packet;
		av_init_packet( &packet );

		if( ! m_inputStream.readNextPacket( packet ) ) // error or end of file
		{
			av_free_packet( &packet );
			return false;
		}

		//avcodec_decode_audio2( m_codecContext, m_frame, &got_frame, &packet );

		av_free_packet( &packet );
	}

	size_t decodedSize = avpicture_get_size( (AVPixelFormat)m_frame->format, m_frame->width, m_frame->height );
	if( frameBuffer.getBuffer().size() != decodedSize )
		frameBuffer.getBuffer().resize( avpicture_get_size( (AVPixelFormat)m_frame->format, m_frame->width, m_frame->height ) );

	// Copy pixel data from an AVPicture into one contiguous buffer.
	avpicture_layout( (AVPicture*)m_frame, (AVPixelFormat)m_frame->format, m_frame->width, m_frame->height, &frameBuffer.getBuffer()[0], frameBuffer.getBuffer().size() );
*/
	return true;
}

}
