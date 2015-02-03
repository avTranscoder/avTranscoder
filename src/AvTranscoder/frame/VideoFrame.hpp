#ifndef _AV_TRANSCODER_FRAME_VIDEO_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_VIDEO_FRAME_HPP_

#include "Frame.hpp"
#include <AvTranscoder/ProfileLoader.hpp>

extern "C" {
#include <libavutil/pixfmt.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>
#include <stdlib.h>

namespace avtranscoder
{

class AvExport VideoFrameDesc
{
public:
	VideoFrameDesc( const size_t width = 0, const size_t height = 0, const AVPixelFormat pixelFormat = AV_PIX_FMT_NONE )
		: _width( width )
		, _height( height )
		, _pixelFormat( pixelFormat )
		, _fps( 1.0 )
	{
	}
	VideoFrameDesc( const size_t width, const size_t height, const std::string& pixelFormat )
		: _width( width )
		, _height( height )
		, _pixelFormat( av_get_pix_fmt( pixelFormat.c_str() ) )
		, _fps( 1.0 )
	{
	}

	size_t getWidth () const { return _width;  }
	size_t getHeight() const { return _height; }
	AVPixelFormat getPixelFormat() const { return _pixelFormat; }
	std::string getPixelFormatName() const
	{
	    const char* formatName = av_get_pix_fmt_name( _pixelFormat );
	    return formatName ? std::string( formatName ) : "unknown pixel format";
	}
	double getFps() const { return _fps; }

	size_t getDataSize() const
	{
		if( _pixelFormat == AV_PIX_FMT_NONE )
			throw std::runtime_error( "incorrect pixel format" );

		size_t size = avpicture_get_size( _pixelFormat, _width, _height );
		if( size == 0 )
			throw std::runtime_error( "unable to determine image buffer size" );

		return size;
	}

	void setWidth ( const size_t width ) { _width = width; }
	void setHeight( const size_t height ) { _height = height; }
	void setPixelFormat( const std::string& pixelFormat ) { _pixelFormat = av_get_pix_fmt( pixelFormat.c_str() ); }
	void setPixelFormat( const AVPixelFormat pixelFormat ) { _pixelFormat = pixelFormat; }
	void setFps( const double fps ) { _fps = fps; }

	void setParameters( const ProfileLoader::Profile& profile )
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

private:
	size_t _width;
	size_t _height;
	AVPixelFormat _pixelFormat;
	double _fps;
};

//template< template<typename> Alloc >
//class AvExport ImageBase
class AvExport VideoFrame : public Frame
{
public:
	VideoFrame( const VideoFrameDesc& ref )
		: Frame( ref.getDataSize(), 0 )
		, _videoFrameDesc( ref )
	{}

	const VideoFrameDesc& desc() const { return _videoFrameDesc; }

private:
	const VideoFrameDesc _videoFrameDesc;
};

//typedef ImageBase<std::allocator> VideoFrame;

}

#endif
