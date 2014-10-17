#ifndef _AV_TRANSCODER_DATA_IMAGE_HPP_
#define _AV_TRANSCODER_DATA_IMAGE_HPP_

#include "Frame.hpp"
#include "Pixel.hpp"
#include <AvTranscoder/Profile.hpp>

extern "C" {
#include <libavutil/pixdesc.h>
#include <libavutil/rational.h>
}

#include <stdexcept>
#include <utility>

namespace avtranscoder
{

// struct ColorProperties
// {
// 	//EColorspace     eColorspace;
// 	AVColorTransferCharacteristic eColorTransfer;
// 	AVColorPrimaries              eColorPrimaries;
// 	//EColorRange     eColorRange;
// };

class AvExport VideoFrameDesc
{
public:
	VideoFrameDesc()
		: _width( 0 )
		, _height( 0 )
		, _pixel()
		, _interlaced( false )
		, _topFieldFirst( false )
	{
		_displayAspectRatio.num = 0;
		_displayAspectRatio.den = 0;
	}
	
	void setWidth ( const size_t width     ) { _width = width; }
	void setHeight( const size_t height    ) { _height = height; }
	void setPixel ( const Pixel  pixel     ) { _pixel = pixel; }
	void setDar( const size_t num, const size_t den ) { _displayAspectRatio.num = num; _displayAspectRatio.den = den; }
	void setDar( const Rational ratio ) { _displayAspectRatio = ratio; }
	
	void setParameters( const Profile::ProfileDesc& desc )
	{
		if( desc.find( constants::avProfilePixelFormat ) != desc.end() )
			setPixel( Pixel( desc.find( constants::avProfilePixelFormat )->second.c_str() ) );
	}

	size_t               getWidth ()    const { return _width;  }
	size_t               getHeight()    const { return _height; }
	Rational getDar() const { return _displayAspectRatio; }
	int getDarNum() const { return _displayAspectRatio.num; }
	int getDarDen() const { return _displayAspectRatio.den; }
	Pixel                getPixelDesc() const { return _pixel; }

	size_t getDataSize() const
	{
		AVPixelFormat pixelFormat = _pixel.findPixel();
		if( pixelFormat == AV_PIX_FMT_NONE )
		{
			throw std::runtime_error( "incorrect pixel description" );
		}
		size_t size = avpicture_get_size( pixelFormat, _width, _height );
		if( size == 0 )
		{
			throw std::runtime_error( "unable to determine image buffer size" );
		}
		return size;
	}

private:
	size_t          _width;
	size_t          _height;
	Rational      _displayAspectRatio;
	Pixel           _pixel;
	// ColorProperties _color;

	bool            _interlaced;
	bool            _topFieldFirst;
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

	const VideoFrameDesc&     desc() const    { return _videoFrameDesc; }

private:
	const VideoFrameDesc _videoFrameDesc;
};

//typedef ImageBase<std::allocator> VideoFrame;

}

#endif
