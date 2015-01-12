#include "ICodec.hpp"

extern "C" {
#include <libavutil/mem.h>
}

#include <stdexcept>
#include <cassert>
#include <iostream>

namespace avtranscoder {

ICodec::ICodec( const ECodecType type, const std::string& codecName )
	: _codecContext( NULL )
	, _avCodec( NULL )
	, _type( type )
{
	setCodec( type, codecName );
}

ICodec::ICodec( const ECodecType type, const AVCodecID codecId )
	: _codecContext( NULL )
	, _avCodec( NULL )
	, _type( type )
{
	setCodec( type, codecId );
}

ICodec::~ICodec()
{
	delete _codecContext;
}

std::string ICodec::getCodecName() const
{
	assert( _avCodec != NULL );
	return avcodec_descriptor_get( _avCodec->id )->name;
}

AVCodecID ICodec::getCodecId() const
{
	assert( _avCodec != NULL );
	return _avCodec->id;
}

int ICodec::getLatency()  const
{
	assert( _codecContext != NULL );
	return _codecContext->getAVCodecContext().delay;
}

void ICodec::setCodec( const ECodecType type, const std::string& codecName )
{
	const AVCodecDescriptor* avCodecDescriptor = avcodec_descriptor_get_by_name( codecName.c_str() );
	if( ! avCodecDescriptor )
	{
		std::string msg( "unable to find codec " );
		msg += codecName;
		throw std::runtime_error( msg );
	}

	setCodec( type, avCodecDescriptor->id );
}

void ICodec::setCodec( const ECodecType type, const AVCodecID codecId )
{
	if( codecId == 0 )
	{
		std::cout << "Warning: Unsupported codec with id 0" << std::endl;
		return;
	}

	avcodec_register_all();

	if( type == eCodecTypeEncoder )
	{
		_avCodec = avcodec_find_encoder( codecId );
		_codecContext = new CodecContext( *_avCodec, AV_OPT_FLAG_ENCODING_PARAM );
	}
	else if( type == eCodecTypeDecoder )
	{
		_avCodec = avcodec_find_decoder( codecId );
		_codecContext = new CodecContext( *_avCodec, AV_OPT_FLAG_DECODING_PARAM );
	}
}

}
