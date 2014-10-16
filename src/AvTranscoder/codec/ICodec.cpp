#include "ICodec.hpp"

extern "C" {
#include <libavutil/mem.h>
}

#include <stdexcept>
#include <cassert>
#include <iostream>

namespace avtranscoder {

ICodec::ICodec( const ECodecType type, const std::string& codecName )
	: _codec( NULL )
	, _codecContext( NULL )
{
	if( codecName.size() )
	{
		setCodec( type, codecName );
	}
}

ICodec::ICodec( const ECodecType type, const AVCodecID codecId )
	: _codec( NULL )
	, _codecContext( NULL )
{
	setCodec( type, codecId );
}

ICodec::~ICodec()
{
	avcodec_close( _codecContext );
	av_free( _codecContext );
	_codecContext = NULL;
}

std::string ICodec::getCodecName() const
{
	assert( _codecContext != NULL );
	return avcodec_descriptor_get( _codecContext->codec_id )->name;
}

AVCodecID ICodec::getCodecId() const
{
	assert( _codecContext != NULL );
	return _codecContext->codec_id;
}

int ICodec::getLatency()  const
{
	assert( _codecContext != NULL );
	return _codecContext->delay;
}

void ICodec::setCodec( const ECodecType type, const std::string& codecName )
{
	avcodec_register_all();  // Warning: should be called only once
	if( type == eCodecTypeEncoder )
		_codec = avcodec_find_encoder_by_name( codecName.c_str() );
	else if( type == eCodecTypeDecoder )
		_codec = avcodec_find_decoder_by_name( codecName.c_str() );
	initCodecContext();
}

void ICodec::setCodec( const ECodecType type, const AVCodecID codecId )
{
	if( codecId == 0 )
	{
		std::cout << "Warning: Unsupported codec with id 0" << std::endl;
		return;
	}
	avcodec_register_all();  // Warning: should be called only once
	if( type == eCodecTypeEncoder )
		_codec = avcodec_find_encoder( codecId );
	else if( type == eCodecTypeDecoder )
		_codec = avcodec_find_decoder( codecId );
	initCodecContext();
}

void ICodec::initCodecContext( )
{
	if( _codec == NULL )
	{
		throw std::runtime_error( "unknown codec" );
	}

	if( ( _codecContext = avcodec_alloc_context3( _codec ) ) == NULL )
	{
		throw std::runtime_error( "unable to allocate codec context and set its fields to default values" );
	}

	// Set default codec parameters
	if( avcodec_get_context_defaults3( _codecContext, _codec ) != 0 )
	{
		throw std::runtime_error( "unable to find codec default values" );
	}
}

}
