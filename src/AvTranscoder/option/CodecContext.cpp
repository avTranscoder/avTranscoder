#include "CodecContext.hpp"

#include <stdexcept>

namespace avtranscoder
{

CodecContext::CodecContext( AVCodec& avCodec, int req_flags )
	: Context( NULL, req_flags )
{
	_avContext = avcodec_alloc_context3( &avCodec );
	if( ! _avContext )
	{
		throw std::runtime_error( "unable to allocate the codecContext and set its fields to default values" );
	}

	// Set default codec parameters
	if( avcodec_get_context_defaults3( &getAVCodecContext(), &avCodec ) != 0 )
	{
		throw std::runtime_error( "unable to find set codecContext to default values corresponding to the given codec" );
	}

	loadOptions( _avContext, req_flags );
}

CodecContext::CodecContext( int req_flags )
	: Context( NULL, req_flags )
{
	_avContext = avcodec_alloc_context3( NULL );
	loadOptions( _avContext, req_flags );
}

CodecContext::~CodecContext()
{
	if( ! _avContext )
		return;
	
	avcodec_close( &getAVCodecContext() );
	av_free( &getAVCodecContext() );
	_avContext = NULL;
}

}
