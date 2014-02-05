#ifndef _AV_TRANSCODER_DATA_IMAGE_HPP_
#define _AV_TRANSCODER_DATA_IMAGE_HPP_

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

class Image
{
public:
	Image()
	{};

	//void alloc() { m_dataBuffer = data; }

	// for overloading get_buffer2
	// int alloc( struct AVCodecContext* s,
	//            AVFrame* frame,
	//            int flags )
	// {

	// }

	void setWidth ( const size_t width     ) { m_width = width; }
	void setHeight( const size_t height    ) { m_height = height; }
	// void setDar   ( const Ratio& dar       ) { m_displayAspectRatio = dar; }
	void setPixel ( const Pixel& pixel     ) { m_pixel = pixel; }

	DataBuffer&          getBuffer() { return m_dataBuffer; }

	unsigned char*       getPtr()    { return &m_dataBuffer[0]; }
	
#ifndef SWIG
	const unsigned char* getPtr()    const { return &m_dataBuffer[0]; }
#endif

	size_t               getSize()   const { return m_dataBuffer.size(); }

	size_t               getWidth () const { return m_width;  }
	size_t               getHeight() const { return m_height; }
	// Ratio                getDar()    const { return m_displayAspectRatio; }
	Pixel                getPixel()  const { return m_pixel; }

private:
	DataBuffer      m_dataBuffer;
	size_t          m_width;
	size_t          m_height;
	// Ratio           m_displayAspectRatio;
	Pixel           m_pixel;
	// ColorProperties m_color;

	bool            m_interlaced;
	bool            m_topFieldFirst;
};

}

#endif
