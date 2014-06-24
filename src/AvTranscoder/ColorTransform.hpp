#ifndef _AV_TRANSCODER_COLOR_TRANSFORM_HPP_
#define _AV_TRANSCODER_COLOR_TRANSFORM_HPP_

#include "DatasStructures/Pixel.hpp"
#include "DatasStructures/Image.hpp"

#include <vector>
#include <string>

class SwsContext;

namespace avtranscoder
{

class AvExport ColorTransform
{
public:
	ColorTransform();

	void convert( const Image& src, Image& dst );

private:
	bool init( const Image& src, const Image& dst );

	SwsContext* _imageConvertContext;

	std::vector<uint8_t *> _srcData;
	std::vector<uint8_t *> _dstData;
	std::vector<int>       _srcLineSize;
	std::vector<int>       _dstLineSize;
	std::vector<size_t>    _srcOffsets;
	std::vector<size_t>    _dstOffsets;

	bool _isInit;
};

}

#endif
