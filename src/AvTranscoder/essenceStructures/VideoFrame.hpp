#ifndef _AV_TRANSCODER_DATA_IMAGE_HPP_
#define _AV_TRANSCODER_DATA_IMAGE_HPP_

#include "Pixel.hpp"
#include "Frame.hpp"
#include <AvTranscoder/Profile.hpp>
#include <AvTranscoder/common.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#ifndef INT64_C
	#define INT64_C(c) (c ## LL)
	#define UINT64_C(c) (c ## ULL)
#endif
#include <libavutil/pixdesc.h>
}

#include <stdexcept>


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
		, _displayAspectRatio()
		, _pixel()
		, _interlaced( false )
		, _topFieldFirst( false )
	{};
	
	void setWidth ( const size_t width     ) { _width = width; }
	void setHeight( const size_t height    ) { _height = height; }
	void setPixel ( const Pixel  pixel     ) { _pixel = pixel; }
	void setDar   ( const size_t num, const size_t den ) { _displayAspectRatio.num = num; _displayAspectRatio.den = den; }
	void setDar   ( const Ratio  ratio     ) { _displayAspectRatio = ratio; }
	
	void setParameters( const Profile::ProfileDesc& desc )
	{
		if( desc.find( Profile::getAvProfilePixelFormatKey() ) != desc.end() )
			setPixel( Pixel( desc.find( Profile::getAvProfilePixelFormatKey() )->second.c_str() ) );
	}

	size_t               getWidth ()    const { return _width;  }
	size_t               getHeight()    const { return _height; }
	Ratio                getDar()       const { return _displayAspectRatio; }
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
	Ratio           _displayAspectRatio;
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
