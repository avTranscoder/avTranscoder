#include "VideoDesc.hpp"

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

VideoDesc::VideoDesc( const std::string& codecName )
	: m_codec( NULL )
	, m_codecContext( NULL )
{
	if( codecName.size() )
		setVideoCodec( codecName );
}

VideoDesc::VideoDesc( const AVCodecID codecId )
	: m_codec( NULL )
	, m_codecContext( NULL )
{
	setVideoCodec( codecId );
}

void VideoDesc::setVideoCodec( const std::string& codecName )
{
	avcodec_register_all();  // Warning: should be called only once
	m_codec = avcodec_find_encoder_by_name( codecName.c_str() );
	initCodecContext();
}

void VideoDesc::setVideoCodec( const AVCodecID codecId )
{
	avcodec_register_all();  // Warning: should be called only once
	m_codec = avcodec_find_encoder( codecId );
	initCodecContext();
}

void VideoDesc::setImageParameters( const Image& image )
{
	setImageParameters( image.desc().getWidth(), image.desc().getHeight(), image.desc().getPixelDesc() );
}


void VideoDesc::setImageParameters( const size_t width, const size_t height, const Pixel& pixel )
{
	setImageParameters( width, height, pixel.findPixel() );
}

void VideoDesc::setImageParameters( const size_t width, const size_t height, const AVPixelFormat& pixel )
{
	m_codecContext->width   = width;
	m_codecContext->height  = height;
	m_codecContext->pix_fmt = pixel;
}

void VideoDesc::setTimeBase( const size_t num, const size_t den )
{
	m_codecContext->time_base.num = num;
	m_codecContext->time_base.den = den;
}

void VideoDesc::initCodecContext( )
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

void VideoDesc::set( const std::string& key, const std::string& flag, const bool enable )
{
	int error = 0;
	int64_t optVal;
	
	const AVOption* flagOpt = av_opt_find( m_codecContext, flag.c_str(), key.c_str(), 0, 0 );

	if( ! flagOpt )
	{
		std::cout << flag << std::endl << " : " << flagOpt->default_val.i64 << std::endl;
		throw std::runtime_error( "unknown flag " + flag );
	}

	error = av_opt_get_int( m_codecContext, key.c_str(), AV_OPT_SEARCH_CHILDREN, &optVal );
	if( error != 0 )
	{
		throw std::runtime_error( "unknown key " + key + ": " + std::string( av_err2str( error ) ) );
	}

	if( enable )
		optVal = optVal |  flagOpt->default_val.i64;
	else
		optVal = optVal &~ flagOpt->default_val.i64;
	
	error = av_opt_set_int( m_codecContext, key.c_str(), optVal, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		throw std::runtime_error( "setting " + key + " parameter to " + flag + ": " + std::string( av_err2str( error ) ) );
	}
}

void VideoDesc::set( const std::string& key, const bool value )
{
	int error = av_opt_set_int( m_codecContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		throw std::runtime_error( "setting " + key + " parameter to " + ( value ? "true" : "false" ) + ": " + std::string( av_err2str( error ) ) );
	}
}

void VideoDesc::set( const std::string& key, const int value )
{
	const AVOption* flagOpt = av_opt_find( m_codecContext, key.c_str(), NULL, 0, AV_OPT_SEARCH_CHILDREN );

	int error = av_opt_set_int( m_codecContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << value;
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + std::string( av_err2str( error ) ) );
	}
}

void VideoDesc::set( const std::string& key, const int num, const int den )
{
	AVRational ratio;
	ratio.num = num;
	ratio.den = den;
	int error = av_opt_set_q( m_codecContext, key.c_str(), ratio, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << num << "/" << den;
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + std::string( av_err2str( error ) ) );
	}
}

void VideoDesc::set( const std::string& key, const double value )
{
	int error = av_opt_set_double( m_codecContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << value;
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + std::string( av_err2str( error ) ) );
	}
}

void VideoDesc::set( const std::string& key, const std::string& value )
{
	int error = av_opt_set( m_codecContext, key.c_str(), value.c_str(), AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		throw std::runtime_error( "setting " + key + " parameter to " + value + ": " + std::string( av_err2str( error ) ) );
	}
}

}