#ifndef _AV_TRANSCODER_DATA_IMAGE_HPP_
#define _AV_TRANSCODER_DATA_IMAGE_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#ifndef INT64_C
	#define INT64_C(c) (c ## LL)
	#define UINT64_C(c) (c ## ULL)
#endif
#include <libavcodec/avcodec.h>
}

#include <iostream>
#include <string>
#include <vector>
#include "Pixel.hpp"

namespace avtranscoder
{

typedef std::vector< unsigned char > DataBuffer;

// struct ColorProperties
// {
// 	//EColorspace     eColorspace;
// 	AVColorTransferCharacteristic eColorTransfer;
// 	AVColorPrimaries              eColorPrimaries;
// 	//EColorRange     eColorRange;
// };

class ImageDesc
{
public:
	ImageDesc()
	{};

	void setWidth ( const size_t width     ) { m_width = width; }
	void setHeight( const size_t height    ) { m_height = height; }
	void setPixel ( const Pixel& pixel     ) { m_pixel = pixel; }
	void setDar   ( const size_t num, const size_t den ) { m_displayAspectRatioNum = num; m_displayAspectRatioDen = den; }

	size_t               getWidth () const { return m_width;  }
	size_t               getHeight() const { return m_height; }
	// Ratio                getDar()    const { return m_displayAspectRatio; }
	Pixel                getPixelDesc() const { return m_pixel; }

	//AVFrame              getEmptyFrame() const;

	size_t getDataSize() const
	{
		return avpicture_get_size( m_pixel.findPixel(), m_width, m_height );
	}

private:

	size_t          m_width;
	size_t          m_height;
	size_t          m_displayAspectRatioNum;
	size_t          m_displayAspectRatioDen;
	// Ratio           m_displayAspectRatio;
	Pixel           m_pixel;
	// ColorProperties m_color;

	bool            m_interlaced;
	bool            m_topFieldFirst;
};

class Image
{
public:
	Image( const ImageDesc& ref )
		: m_dataBuffer( ref.getDataSize(), 0 )
		, m_imageDesc( ref )
	{ }

	const ImageDesc&     desc() const    { return m_imageDesc; }
	DataBuffer&          getBuffer()     { return m_dataBuffer; }
	unsigned char*       getPtr()        { return &m_dataBuffer[0]; }
#ifndef SWIG
	const unsigned char* getPtr()  const { return &m_dataBuffer[0]; }
#endif
	size_t               getSize() const { return m_dataBuffer.size(); }

	//void alloc() { m_dataBuffer = data; }

	// for overloading get_buffer2
	// int alloc( struct AVCodecContext* s,
	//            AVFrame* frame,
	//            int flags )
	// {

	// }

private:
	DataBuffer m_dataBuffer;
	const ImageDesc m_imageDesc;
};

}

#endif
