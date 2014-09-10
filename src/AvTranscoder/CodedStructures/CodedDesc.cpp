#include "CodedDesc.hpp"

#include <stdexcept>
#include <cassert>

namespace avtranscoder {

CodedDesc::CodedDesc( const std::string& codecName )
	: m_codec( NULL )
	, m_codecContext( NULL )
{
	if( codecName.size() )
		setCodec( codecName );
}

CodedDesc::CodedDesc( const AVCodecID codecId )
	: m_codec( NULL )
	, m_codecContext( NULL )
{
	setCodec( codecId );
}

CodedDesc::CodedDesc( AVCodec& avCodec, AVCodecContext& avCodecContext )
	: m_codec( &avCodec )
	, m_codecContext( &avCodecContext )
{
	
}

std::string CodedDesc::getCodecName() const
{
	assert( m_codecContext != NULL );
	return avcodec_descriptor_get( m_codecContext->codec_id )->name;
}

AVCodecID CodedDesc::getCodecId() const
{
	assert( m_codecContext != NULL );
	return m_codecContext->codec_id;
}

int CodedDesc::getLatency()  const
{
	assert( m_codecContext != NULL );
	return m_codecContext->delay;
}

void CodedDesc::setCodec( const std::string& codecName )
{
	avcodec_register_all();  // Warning: should be called only once
	m_codec = avcodec_find_encoder_by_name( codecName.c_str() );
	initCodecContext();
}

void CodedDesc::setCodec( const AVCodecID codecId )
{
	avcodec_register_all();  // Warning: should be called only once
	m_codec = avcodec_find_encoder( codecId );
	initCodecContext();
}

void CodedDesc::initCodecContext( )
{
	if( m_codec == NULL )
	{
		throw std::runtime_error( "unknown codec" );
	}

	if( ( m_codecContext = avcodec_alloc_context3( m_codec ) ) == NULL )
	{
		throw std::runtime_error( "unable to create context for context" );
	}

	// Set default codec parameters
	if( avcodec_get_context_defaults3( m_codecContext, m_codec ) != 0 )
	{
		throw std::runtime_error( "unable to find codec default values" );
	}
}

}
