#include "CodedDesc.hpp"

#include <stdexcept>
#include <cassert>

namespace avtranscoder {

CodedDesc::CodedDesc( const std::string& codecName )
	: _codec( NULL )
	, _codecContext( NULL )
{
	if( codecName.size() )
		setCodec( codecName );
}

CodedDesc::CodedDesc( const AVCodecID codecId )
	: _codec( NULL )
	, _codecContext( NULL )
{
	setCodec( codecId );
}

CodedDesc::CodedDesc( AVCodec& avCodec, AVCodecContext& avCodecContext )
	: _codec( &avCodec )
	, _codecContext( &avCodecContext )
{
	
}

std::string CodedDesc::getCodecName() const
{
	assert( _codecContext != NULL );
	return avcodec_descriptor_get( _codecContext->codec_id )->name;
}

AVCodecID CodedDesc::getCodecId() const
{
	assert( _codecContext != NULL );
	return _codecContext->codec_id;
}

int CodedDesc::getLatency()  const
{
	assert( _codecContext != NULL );
	return _codecContext->delay;
}

void CodedDesc::setCodec( const std::string& codecName )
{
	avcodec_register_all();  // Warning: should be called only once
	_codec = avcodec_find_encoder_by_name( codecName.c_str() );
	initCodecContext();
}

void CodedDesc::setCodec( const AVCodecID codecId )
{
	avcodec_register_all();  // Warning: should be called only once
	_codec = avcodec_find_encoder( codecId );
	initCodecContext();
}

void CodedDesc::initCodecContext( )
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
