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

#include <iostream>

namespace avtranscoder
{

InputStreamVideo::InputStreamVideo()
	: m_formatContext( NULL )
	, m_codec        ( NULL )
	, m_codecContext ( NULL )
	, m_frame        ( NULL )
	, m_selectedStream( -1 )
{}

InputStreamVideo::~InputStreamVideo()
{
	if( m_formatContext )
	{
		avformat_close_input( &m_formatContext );
		m_formatContext = NULL;
	}
	if( m_codecContext )
	{
		avcodec_close( m_codecContext );
		m_codecContext = NULL;
	}
	if( m_frame != NULL )
	{
		av_frame_free( &m_frame );
		m_frame = NULL;
	}
}

bool InputStreamVideo::setup( const std::string& filename, const size_t streamIndex )
{
	av_register_all();

	if( avformat_open_input( &m_formatContext, filename.c_str(), NULL, NULL ) < 0 )
	{
		return false;
	}

	// update format context informations from streams
	if( avformat_find_stream_info( m_formatContext, NULL ) < 0 )
	{
		return false;
	}

	size_t videoStreamCount = 0;
	for( size_t streamId = 0; streamId < m_formatContext->nb_streams; streamId++ )
	{
		if( m_formatContext->streams[streamId]->codec->codec_type == AVMEDIA_TYPE_VIDEO )
		{
			if( videoStreamCount == streamIndex )
			{
				m_selectedStream = streamId;
			}
			videoStreamCount++;
		}
	}

	if( m_selectedStream == -1 )
		return false;

	m_codec = avcodec_find_decoder( m_formatContext->streams[m_selectedStream]->codec->codec_id );
	if( m_codec == NULL )
	{
		return false;
	}

	m_codecContext = avcodec_alloc_context3( m_codec );

	if( m_codecContext == NULL )
	{
		return false;
	}

	// if( codec->capabilities & CODEC_CAP_TRUNCATED )
	// 	codecContext->flags|= CODEC_FLAG_TRUNCATED;

	avcodec_open2( m_codecContext, m_codec, NULL );

	if( m_codecContext == NULL || m_codec == NULL )
	{
		return false;
	}

	m_frame = avcodec_alloc_frame();
	if( m_frame == NULL )
	{
		return false;
	}

	return true;
}

bool InputStreamVideo::readNextCodedFrame( DataStream& frameBuffer )
{
	bool gotData = false;

	AVPacket pkt;
	av_init_packet( &pkt );
	
	while( ! gotData )
	{
		int ret = av_read_frame( m_formatContext, &pkt );
		if( ret < 0 ) // error or end of file
		{
			av_free_packet( &pkt );
			return false;
		}

		// We only read one stream and skip others
		if( pkt.stream_index == m_selectedStream )
		{
			gotData = true;
		}
	}

	frameBuffer.getBuffer().resize( pkt.size );
	memcpy( frameBuffer.getPtr(), pkt.data, pkt.size );

	av_free_packet( &pkt );

	return true;
}

bool InputStreamVideo::readNextFrame( Image& frameBuffer )
{
	int got_frame = 0;

	while( ! got_frame )
	{
		AVPacket pkt;
		av_init_packet( &pkt );
		int ret = av_read_frame( m_formatContext, &pkt );
		if( ret < 0 ) // error or end of file
		{
			av_free_packet( &pkt );
			return false;
		}

		// We only read one stream and skip others
		if( pkt.stream_index == m_selectedStream )
		{
			avcodec_decode_video2( m_codecContext, m_frame, &got_frame, &pkt );
		}
		av_free_packet( &pkt );
	}

	size_t decodedSize = avpicture_get_size( (AVPixelFormat)m_frame->format, m_frame->width, m_frame->height );
	if( frameBuffer.getBuffer().size() != decodedSize )
		frameBuffer.getBuffer().resize( avpicture_get_size( (AVPixelFormat)m_frame->format, m_frame->width, m_frame->height ) );

	// Copy pixel data from an AVPicture into one contiguous buffer.
	avpicture_layout( (AVPicture*)m_frame, (AVPixelFormat)m_frame->format, m_frame->width, m_frame->height, &frameBuffer.getBuffer()[0], frameBuffer.getBuffer().size() );

	return true;
}

}