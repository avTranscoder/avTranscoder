#include "CodecContext.hpp"

#include <stdexcept>

namespace avtranscoder
{

CodecContext::CodecContext( AVCodec& avCodec, int req_flags )
	: _avCodecContext( NULL )
	, _options()
{
	_avCodecContext = avcodec_alloc_context3( &avCodec );
	if( ! _avCodecContext )
	{
		throw std::runtime_error( "unable to allocate the codecContext and set its fields to default values" );
	}

	// Set default codec parameters
	if( avcodec_get_context_defaults3( &getAVCodecContext(), &avCodec ) != 0 )
	{
		throw std::runtime_error( "unable to find set codecContext to default values corresponding to the given codec" );
	}

	loadOptions( _options, _avCodecContext, req_flags );
}

CodecContext::CodecContext( int req_flags )
	: _avCodecContext( NULL )
	, _options()
{
	_avCodecContext = avcodec_alloc_context3( NULL );
	loadOptions( _options, _avCodecContext, req_flags );
}

CodecContext::~CodecContext()
{
	if( ! _avCodecContext )
		return;
	
	avcodec_close( &getAVCodecContext() );
	av_free( &getAVCodecContext() );
	_avCodecContext = NULL;
}

std::vector<Option> CodecContext::getOptions()
{
	std::vector<Option> optionsArray;
	for( OptionMap::iterator it = _options.begin(); it != _options.end(); ++it )
	{
		optionsArray.push_back( it->second );
	}
	return optionsArray;
}

}
