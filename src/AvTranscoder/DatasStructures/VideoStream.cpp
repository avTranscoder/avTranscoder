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
#include <libavutil/opt.h>
}
#include <iostream>
#include <stdexcept>
#include <sstream>

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
	m_codecContext->width   = image.desc().getWidth();
	m_codecContext->height  = image.desc().getHeight();
	m_codecContext->pix_fmt = image.desc().getPixelDesc().findPixel();
}

void VideoStream::setBitrate( const size_t bitRate )
{
	set( "b", (int)bitRate );
	//m_codecContext->bit_rate = bitRate;
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

void VideoStream::set( const std::string& key, const bool value )
{
	int error = av_opt_set_int( m_codecContext, key.c_str(), value, 0 );
	if( error != 0 )
	{
		throw std::runtime_error( "setting " + key + " parameter to " + ( value ? "true" : "false" ) + ": " + std::string( av_err2str( error ) ) );
	}
}

void VideoStream::set( const std::string& key, const int value )
{
	int error = av_opt_set_int( m_codecContext, key.c_str(), value, 0 );
	if( error != 0 )
	{
		std::ostringstream os;
		os << value;
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + std::string( av_err2str( error ) ) );
	}
}

void VideoStream::set( const std::string& key, const int num, const int den )
{
	AVRational ratio;
	ratio.num = num;
	ratio.den = den;
	int error = av_opt_set_q( m_codecContext, key.c_str(), ratio, 0 );
	if( error != 0 )
	{
		std::ostringstream os;
		os << num << "/" << den;
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + std::string( av_err2str( error ) ) );
	}
}

void VideoStream::set( const std::string& key, const double value )
{
	int error = av_opt_set_double( m_codecContext, key.c_str(), value, 0 );
	if( error != 0 )
	{
		std::ostringstream os;
		os << value;
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + std::string( av_err2str( error ) ) );
	}
}

void VideoStream::set( const std::string& key, const std::string& value )
{
	int error = av_opt_set( m_codecContext, key.c_str(), value.c_str(), 0 );
	if( error != 0 )
	{
		throw std::runtime_error( "setting " + key + " parameter to " + value + ": " + std::string( av_err2str( error ) ) );
	}
}

}