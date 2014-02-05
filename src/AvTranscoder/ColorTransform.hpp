#ifndef _AV_TRANSCODER_COLOR_TRANSFORM_HPP_
#define _AV_TRANSCODER_COLOR_TRANSFORM_HPP_

#include "DatasStructures/Pixel.hpp"
#include "DatasStructures/Image.hpp"

#include <vector>
#include <string>

namespace avtranscoder
{

class ColorTransform
{
public:
	ColorTransform();

	void setWidth ( const size_t width  );
	void setHeight( const size_t height );

	void setInputPixel ( const Pixel& pixel ); // ColorProfile
	void setOutputPixel( const Pixel& pixel );

	bool init();

	void convert( const Image& src, Image& dst );


private:
	size_t m_width;
	size_t m_height;

	Pixel m_inputPixel;
	Pixel m_outputPixel;
};

}

#endif
