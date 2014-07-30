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
		: m_width( 0 )
		, m_height( 0 )
		, m_displayAspectRatio()
		, m_pixel()
		, m_interlaced( false )
		, m_topFieldFirst( false )
	{};
	
	void setWidth ( const size_t width     ) { m_width = width; }
	void setHeight( const size_t height    ) { m_height = height; }
	void setPixel ( const Pixel  pixel     ) { m_pixel = pixel; }
	void setDar   ( const size_t num, const size_t den ) { m_displayAspectRatio.num = num; m_displayAspectRatio.den = den; }
	void setDar   ( const Ratio  ratio     ) { m_displayAspectRatio = ratio; }
	
	void setParameters( const Profile::ProfileDesc& desc )
	{
		if( desc.find( Profile::avProfilePixelFormat ) != desc.end() )
			setPixel( Pixel( desc.find( Profile::avProfilePixelFormat )->second.c_str() ) );
	}

	size_t               getWidth ()    const { return m_width;  }
	size_t               getHeight()    const { return m_height; }
	Ratio                getDar()       const { return m_displayAspectRatio; }
	Pixel                getPixelDesc() const { return m_pixel; }

	size_t getDataSize() const
	{
		AVPixelFormat pixelFormat = m_pixel.findPixel();
		if( pixelFormat == AV_PIX_FMT_NONE )
		{
			throw std::runtime_error( "incorrect pixel description" );
		}
		size_t size = avpicture_get_size( pixelFormat, m_width, m_height );
		if( size == 0 )
		{
			throw std::runtime_error( "unable to determine image buffer size" );
		}
		return size;
	}

private:
	size_t          m_width;
	size_t          m_height;
	Ratio           m_displayAspectRatio;
	Pixel           m_pixel;
	// ColorProperties m_color;

	bool            m_interlaced;
	bool            m_topFieldFirst;
};

//template< template<typename> Alloc >
//class AvExport ImageBase
class AvExport VideoFrame : public Frame
{
public:
	VideoFrame( const VideoFrameDesc& ref )
		: m_videoFrameDesc( ref )
	{
		m_dataBuffer = DataBuffer( ref.getDataSize(), 0 );
	}

	virtual ~VideoFrame()
	{};

	const VideoFrameDesc&     desc() const    { return m_videoFrameDesc; }

private:
	const VideoFrameDesc m_videoFrameDesc;
};

//typedef ImageBase<std::allocator> VideoFrame;

}

#endif
