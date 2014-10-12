#include "ICodec.hpp"

#include <stdexcept>
#include <cassert>

namespace avtranscoder {

ICodec::ICodec( const ECodecType type, const std::string& codecName )
	: _codec( NULL )
	, _codecContext( NULL )
{
	if( codecName.size() )
	{
		if( type == eCodecTypeEncoder )
			setEncoderCodec( codecName );
		else if( type == eCodecTypeDecoder )
			setDecoderCodec( codecName );
	}
}

ICodec::ICodec( const ECodecType type, const AVCodecID codecId )
	: _codec( NULL )
	, _codecContext( NULL )
{
	if( type == eCodecTypeEncoder )
		setEncoderCodec( codecId );
	else if( type == eCodecTypeDecoder )
		setDecoderCodec( codecId );
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

void ICodec::setEncoderCodec( const std::string& codecName )
{
	avcodec_register_all();  // Warning: should be called only once
	_codec = avcodec_find_encoder_by_name( codecName.c_str() );
	initCodecContext();
}

void ICodec::setEncoderCodec( const AVCodecID codecId )
{
	avcodec_register_all();  // Warning: should be called only once
	_codec = avcodec_find_encoder( codecId );
	initCodecContext();
}

void ICodec::setDecoderCodec( const std::string& codecName )
{
	avcodec_register_all();  // Warning: should be called only once
	_codec = avcodec_find_decoder_by_name( codecName.c_str() );
	initCodecContext();
}

void ICodec::setDecoderCodec( const AVCodecID codecId )
{
	avcodec_register_all();  // Warning: should be called only once
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
		throw std::runtime_error( "unable to create context for context" );
	}

	// Set default codec parameters
	if( avcodec_get_context_defaults3( _codecContext, _codec ) != 0 )
	{
		throw std::runtime_error( "unable to find codec default values" );
	}
}

}
