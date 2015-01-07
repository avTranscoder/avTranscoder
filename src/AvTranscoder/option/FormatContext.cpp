#include "FormatContext.hpp"

#include <stdexcept>
#include <sstream>
#include <iostream>

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
		avformat_free_context( &getAVFormatContext() );
}

void FormatContext::findStreamInfo( AVDictionary** options )
{
	int err = avformat_find_stream_info( &getAVFormatContext(), options );
	if( err < 0 )
	{
		avformat_close_input( reinterpret_cast<AVFormatContext**>( &_avContext ) );
		throw std::ios_base::failure( "unable to find stream informations" );
	}
}

void FormatContext::openRessource( const std::string& url, int flags )
{
	if( ( getAVFormatContext().flags & AVFMT_NOFILE ) == AVFMT_NOFILE )
		return;

	int err = avio_open2( &getAVFormatContext().pb, url.c_str(), flags, NULL, NULL );
	if( err < 0 )
	{
		avformat_close_input( reinterpret_cast<AVFormatContext**>( &_avContext ) );
		throw std::ios_base::failure( "error when opening output format" );
	}
}

void FormatContext::closeRessource()
{
	if( ( getAVFormatContext().flags & AVFMT_NOFILE ) == AVFMT_NOFILE )
		return;

	int err = avio_close( &getAVIOContext() );
	if( err < 0 )
	{
		avformat_close_input( reinterpret_cast<AVFormatContext**>( &_avContext ) );
		throw std::ios_base::failure( "error when close output format" );
	}
}

void FormatContext::writeHeader( AVDictionary** options )
{
	int ret = avformat_write_header( &getAVFormatContext(), options );
	if( ret != 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( ret, err, sizeof(err) );
		std::string msg = "could not write header: ";
		msg += err;
		throw std::runtime_error( msg );
	}
}

void FormatContext::writeFrame( AVPacket& packet, bool interleaved )
{
	int ret = 0;
	if( interleaved )
		ret = av_interleaved_write_frame( &getAVFormatContext(), &packet );
	else
		ret = av_write_frame( &getAVFormatContext(), &packet );
	
	if( ret != 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( ret, err, sizeof(err) );
		std::string msg = "error when writting packet in stream: ";
		msg += err;
		throw std::runtime_error( msg );
	}
}

void FormatContext::writeTrailer()
{
	if( av_write_trailer( &getAVFormatContext() ) != 0)
	{
		throw std::runtime_error( "could not write trailer" );
	}
}

void FormatContext::addMetaData( const std::string& key, const std::string& value )
{
	int ret = av_dict_set( &getAVFormatContext().metadata, key.c_str(), value.c_str(), 0 );
	if( ret < 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( ret, err, sizeof(err) );
		std::cout << err << std::endl;
	}
}

AVStream& FormatContext::addAVStream( AVCodec& avCodec )
{
	AVStream* stream = avformat_new_stream( &getAVFormatContext(), &avCodec );
	if( stream == NULL )
	{
		throw std::runtime_error( "unable to add new video stream" );
	}
	return *stream;
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

void FormatContext::setOutputFormat( const std::string& filename, const std::string& shortName, const std::string& mimeType )
{
	AVOutputFormat* oformat = av_guess_format( shortName.c_str(), filename.c_str(), mimeType.c_str() );
	if( ! oformat )
		throw std::ios_base::failure( "unable to find format" );

	getAVFormatContext().oformat = oformat;
}

}
