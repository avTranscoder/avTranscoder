#include "FormatContext.hpp"

#include <stdexcept>
#include <sstream>

namespace avtranscoder
{

FormatContext::FormatContext( const std::string& filename, int req_flags )
	: Context( NULL, req_flags )
	, _isOpen( true )
{
	av_register_all();  // TODO: call it once

	int err = avformat_open_input( reinterpret_cast<AVFormatContext**>( &_avContext ), filename.c_str(), NULL, NULL );
	if( err < 0 )
	{
		std::string msg = "unable to open file: ";
		msg += filename;
		throw std::ios_base::failure( msg );
	}
	loadOptions( _avContext, req_flags );
}

FormatContext::FormatContext( int req_flags )
	: Context( NULL, req_flags )
	, _isOpen( false )
{
	av_register_all();  // TODO: call it once

	_avContext = avformat_alloc_context();
	loadOptions( _avContext, req_flags );
}

FormatContext::~FormatContext()
{
	if( ! _avContext )
		return;
	
	if( _isOpen )
		avformat_close_input( reinterpret_cast<AVFormatContext**>( &_avContext ) );
	else
		avformat_free_context( static_cast<AVFormatContext*>( _avContext ) );
}

void FormatContext::findStreamInfo( AVDictionary** options )
{
	int err = avformat_find_stream_info( static_cast<AVFormatContext*>( _avContext ), options );
	if( err < 0 )
	{
		avformat_close_input( reinterpret_cast<AVFormatContext**>( &_avContext ) );
		throw std::ios_base::failure( "unable to find stream informations" );
	}
}

AVStream& FormatContext::getAVStream( size_t index ) const
{
	if( index >= getNbStreams() )
	{
		std::stringstream msg;
		msg << "Can't acces stream at index ";
		msg << index;
		throw std::runtime_error( msg.str() );
	}
	return *getAVFormatContext().streams[index];
}

}
