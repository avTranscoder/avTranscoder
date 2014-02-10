#ifndef _AV_TRANSCODER_COLOR_TRANSFORM_HPP_
#define _AV_TRANSCODER_COLOR_TRANSFORM_HPP_

#include "DatasStructures/Pixel.hpp"
#include "DatasStructures/Image.hpp"

#include <vector>
#include <string>

class SwsContext;

namespace avtranscoder
{

class ColorTransform
{
public:
	ColorTransform();

	void convert( const Image& src, Image& dst );

private:
	bool init( const Image& src, const Image& dst );

	SwsContext* m_imageConvertContext;

	std::vector<uint8_t *> srcData;
	std::vector<uint8_t *> dstData;
	std::vector<int>       srcLineSize;
	std::vector<int>       dstLineSize;
	std::vector<size_t>    srcOffsets;
	std::vector<size_t>    dstOffsets;

	bool m_isInit;
};

}

#endif
