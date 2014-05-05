#include "AudioDesc.hpp"

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
#include <libavutil/error.h>
}
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace avtranscoder
{

AudioDesc::AudioDesc( const std::string& codecName )
	: m_codec( NULL )
	, m_codecContext( NULL )
{
	if( codecName.size() )
		setAudioCodec( codecName );
}

AudioDesc::AudioDesc( const AVCodecID codecId )
	: m_codec( NULL )
	, m_codecContext( NULL )
{
	setAudioCodec( codecId );
}

void AudioDesc::setAudioCodec( const std::string& codecName )
{
	avcodec_register_all();  // Warning: should be called only once
	m_codec = avcodec_find_encoder_by_name( codecName.c_str() );
	initCodecContext();
}

void AudioDesc::setAudioCodec( const AVCodecID codecId )
{
	avcodec_register_all();  // Warning: should be called only once
	m_codec = avcodec_find_encoder( codecId );
	initCodecContext();
}

void AudioDesc::setAudioParameters( const size_t sampleRate, const size_t channels, const AVSampleFormat& sampleFormat )
{
	m_codecContext->sample_rate = sampleRate;
	m_codecContext->channels    = channels;
	m_codecContext->sample_fmt  = sampleFormat;
}

void AudioDesc::initCodecContext( )
{
	if( m_codec == NULL )
	{
		throw std::runtime_error( "unknown audio codec" );
	}

	if( ( m_codecContext = avcodec_alloc_context3( m_codec ) ) == NULL )
	{
		throw std::runtime_error( "unable to create context for audio context" );
	}

	// Set default codec parameters
	if( avcodec_get_context_defaults3( m_codecContext, m_codec ) != 0 )
	{
		throw std::runtime_error( "unable to find audio codec default values" );
	}
}

void AudioDesc::set( const std::string& key, const std::string& flag, const bool enable )
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
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "unknown key " + key + ": " + err );
	}

	if( enable )
		optVal = optVal |  flagOpt->default_val.i64;
	else
		optVal = optVal &~ flagOpt->default_val.i64;
	
	error = av_opt_set_int( m_codecContext, key.c_str(), optVal, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + flag + ": " + err );
	}
}

void AudioDesc::set( const std::string& key, const bool value )
{
	int error = av_opt_set_int( m_codecContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + ( value ? "true" : "false" ) + ": " + err );
	}
}

void AudioDesc::set( const std::string& key, const int value )
{
	//const AVOption* flagOpt = av_opt_find( m_codecContext, key.c_str(), NULL, 0, AV_OPT_SEARCH_CHILDREN );

	int error = av_opt_set_int( m_codecContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << value;
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + err );
	}
}

void AudioDesc::set( const std::string& key, const int num, const int den )
{
	AVRational ratio;
	ratio.num = num;
	ratio.den = den;
	int error = av_opt_set_q( m_codecContext, key.c_str(), ratio, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << num << "/" << den;
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + err );
	}
}

void AudioDesc::set( const std::string& key, const double value )
{
	int error = av_opt_set_double( m_codecContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << value;
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + err );
	}
}

void AudioDesc::set( const std::string& key, const std::string& value )
{
	int error = av_opt_set( m_codecContext, key.c_str(), value.c_str(), AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + value + ": " + err );
	}
}

std::string AudioDesc::getAudioCodec() const
{
	return m_codecContext->codec_name;
}

AVCodecID AudioDesc::getAudioCodecId() const
{
	return m_codecContext->codec_id;
}

}
