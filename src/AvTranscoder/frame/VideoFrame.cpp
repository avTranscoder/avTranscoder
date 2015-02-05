#include "VideoFrame.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>
#include <stdlib.h>

namespace avtranscoder
{

VideoFrameDesc::VideoFrameDesc( const size_t width, const size_t height, const AVPixelFormat pixelFormat )
	: _width( width )
	, _height( height )
	, _pixelFormat( pixelFormat )
	, _fps( 1.0 )
{
}
VideoFrameDesc::VideoFrameDesc( const size_t width, const size_t height, const std::string& pixelFormat )
	: _width( width )
	, _height( height )
	, _pixelFormat( av_get_pix_fmt( pixelFormat.c_str() ) )
	, _fps( 1.0 )
{
}

std::string VideoFrameDesc::getPixelFormatName() const
{
	const char* formatName = av_get_pix_fmt_name( _pixelFormat );
	return formatName ? std::string( formatName ) : "unknown pixel format";
}

size_t VideoFrameDesc::getDataSize() const
{
	if( _pixelFormat == AV_PIX_FMT_NONE )
		throw std::runtime_error( "incorrect pixel format" );

	size_t size = avpicture_get_size( _pixelFormat, _width, _height );
	if( size == 0 )
		throw std::runtime_error( "unable to determine image buffer size" );

	return size;
}

void VideoFrameDesc::setPixelFormat( const std::string& pixelFormat )
{
	_pixelFormat = av_get_pix_fmt( pixelFormat.c_str() );
}

void VideoFrameDesc::setParameters( const ProfileLoader::Profile& profile )
{
	// width
	if( profile.count( constants::avProfileWidth ) )
		setWidth( atoi( profile.find( constants::avProfileWidth )->second.c_str() ) );
	// height
	if( profile.count( constants::avProfileHeight ) )
		setHeight( atoi( profile.find( constants::avProfileHeight )->second.c_str() ) );
	// pixel format	
	if( profile.count( constants::avProfilePixelFormat ) )
		setPixelFormat( profile.find( constants::avProfilePixelFormat )->second );
	// fps
	if( profile.count( constants::avProfileFrameRate ) )
		setFps( atof( profile.find( constants::avProfileFrameRate )->second.c_str() ) );
}

}


