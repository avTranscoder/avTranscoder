#include "VideoStream.hpp"

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

VideoStream::VideoStream( const std::string& codecName )
	: m_codec( NULL )
	, m_codecContext( NULL )
{
	if( codecName.size() )
		setVideoCodec( codecName );
}

VideoStream::VideoStream( const AVCodecID codecId )
	: m_codec( NULL )
	, m_codecContext( NULL )
{
	setVideoCodec( codecId );
}

void VideoStream::setVideoCodec( const std::string& codecName )
{
	avcodec_register_all();  // Warning: should be called only once
	m_codec = avcodec_find_encoder_by_name( codecName.c_str() );
	initCodecContext();
}

void VideoStream::setVideoCodec( const AVCodecID codecId )
{
	avcodec_register_all();  // Warning: should be called only once
	m_codec = avcodec_find_encoder( codecId );
	initCodecContext();
}

void VideoStream::setParametersFromImage( const Image& image )
{
	m_codecContext->width   = image.getWidth();
	m_codecContext->height  = image.getHeight();
	m_codecContext->pix_fmt = image.getPixelDesc().findPixel();
	std::cout << image.getPixelDesc().findPixel() << std::endl;
}

void VideoStream::setBitrate( const size_t bitRate )
{
	m_codecContext->bit_rate = bitRate;
}

void VideoStream::setTimeBase( const size_t num, const size_t den )
{
	m_codecContext->time_base.num = num;
	m_codecContext->time_base.den = den;
}

void VideoStream::initCodecContext( )
{
	if( m_codec == NULL )
		return;

	if( ( m_codecContext = avcodec_alloc_context3( m_codec ) ) == NULL )
		return;

	// Set default codec parameters
	if( avcodec_get_context_defaults3( m_codecContext, m_codec ) != 0 )
	{
		m_codecContext = NULL;
		return;
	}
}

}