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
#include <utility>

namespace avtranscoder
{

class AvExport VideoFrameDesc
{
public:
	VideoFrameDesc( const size_t width = 0, const size_t height = 0, const AVPixelFormat pixelFormat = AV_PIX_FMT_NONE )
		: _width( width )
		, _height( height )
		, _pixelFormat( pixelFormat )
		, _interlaced( false )
		, _topFieldFirst( false )
	{
		_displayAspectRatio.num = width;
		_displayAspectRatio.den = height;
	}
	VideoFrameDesc( const size_t width, const size_t height, const std::string& pixelFormat )
		: _width( width )
		, _height( height )
		, _pixelFormat( av_get_pix_fmt( pixelFormat.c_str() ) )
		, _interlaced( false )
		, _topFieldFirst( false )
	{
		_displayAspectRatio.num = width;
		_displayAspectRatio.den = height;
	}

	size_t getWidth () const { return _width;  }
	size_t getHeight() const { return _height; }
	Rational getDar() const { return _displayAspectRatio; }
	int getDarNum() const { return _displayAspectRatio.num; }
	int getDarDen() const { return _displayAspectRatio.den; }
	AVPixelFormat getPixelFormat() const { return _pixelFormat; }
	std::string getPixelFormatName() const
	{
	    const char* pixelName = av_get_pix_fmt_name( _pixelFormat );
	    if( pixelName )
		return std::string( pixelName );
	    return std::string( "unknown pixel format" );
	}

	size_t getDataSize() const
	{
		if( _pixelFormat == AV_PIX_FMT_NONE )
		{
			throw std::runtime_error( "incorrect pixel description" );
		}
		size_t size = avpicture_get_size( _pixelFormat, _width, _height );
		if( size == 0 )
		{
			throw std::runtime_error( "unable to determine image buffer size" );
		}
		return size;
	}

	void setWidth ( const size_t width ) { _width = width; }
	void setHeight( const size_t height ) { _height = height; }
	void setPixelFormat( const std::string& pixelFormat ) { _pixelFormat = av_get_pix_fmt( pixelFormat.c_str() ); }
	void setPixelFormat( const AVPixelFormat pixelFormat ) { _pixelFormat = pixelFormat; }
	void setDar( const size_t num, const size_t den ) { _displayAspectRatio.num = num; _displayAspectRatio.den = den; }
	void setDar( const Rational& ratio ) { _displayAspectRatio = ratio; }

	void setParameters( const ProfileLoader::Profile& profile )
	{
		if( profile.find( constants::avProfilePixelFormat ) != profile.end() )
			setPixelFormat( profile.find( constants::avProfilePixelFormat )->second );
	}

private:
	size_t _width;
	size_t _height;
	Rational  _displayAspectRatio;
	AVPixelFormat _pixelFormat;
	bool _interlaced;
	bool _topFieldFirst;
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
