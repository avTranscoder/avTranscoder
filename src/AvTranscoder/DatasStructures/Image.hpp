#ifndef _AV_TRANSCODER_DATA_IMAGE_HPP_
#define _AV_TRANSCODER_DATA_IMAGE_HPP_

#include <AvTranscoder/common.hpp>

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
	void setDar   ( const size_t num, const size_t den ) { m_displayAspectRatio.num = num; m_displayAspectRatio.den = den; }
	void setDar   ( const Ratio ratio ) { m_displayAspectRatio = ratio; }

	size_t               getWidth ()    const { return m_width;  }
	size_t               getHeight()    const { return m_height; }
	Ratio                getDar()       const { return m_displayAspectRatio; }
	Pixel                getPixelDesc() const { return m_pixel; }

	size_t getDataSize() const
	{
		return avpicture_get_size( m_pixel.findPixel(), m_width, m_height );
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

private:
	DataBuffer m_dataBuffer;
	const ImageDesc m_imageDesc;
};

}

#endif
