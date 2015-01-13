#ifndef _AV_TRANSCODER_FRAME_VIDEO_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_VIDEO_FRAME_HPP_

#include "Frame.hpp"
#include <AvTranscoder/ProfileLoader.hpp>

extern "C" {
#include <libavutil/rational.h>
#include <libavutil/pixfmt.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>

namespace avtranscoder
{

class AvExport VideoFrameDesc
{
public:
	VideoFrameDesc( const size_t width = 0, const size_t height = 0, const AVPixelFormat pixelFormat = AV_PIX_FMT_NONE )
		: _width( width )
		, _height( height )
		, _pixelFormat( pixelFormat )
	{
	}
	VideoFrameDesc( const size_t width, const size_t height, const std::string& pixelFormat )
		: _width( width )
		, _height( height )
		, _pixelFormat( av_get_pix_fmt( pixelFormat.c_str() ) )
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

	void setParameters( const ProfileLoader::Profile& profile )
	{
		if( profile.find( constants::avProfilePixelFormat ) != profile.end() )
			setPixelFormat( profile.find( constants::avProfilePixelFormat )->second );
	}

private:
	size_t _width;
	size_t _height;
	AVPixelFormat _pixelFormat;
};

//template< template<typename> Alloc >
//class AvExport ImageBase
class AvExport VideoFrame : public Frame
{
public:
	VideoFrame( const VideoFrameDesc& ref )
		: _videoFrameDesc( ref )
	{
		_dataBuffer = DataBuffer( ref.getDataSize(), 0 );
	}

	const VideoFrameDesc& desc() const { return _videoFrameDesc; }

private:
	const VideoFrameDesc _videoFrameDesc;
};

//typedef ImageBase<std::allocator> VideoFrame;

}

#endif
