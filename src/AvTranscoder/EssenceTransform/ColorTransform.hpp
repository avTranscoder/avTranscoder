#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_COLOR_TRANSFORM_HPP_
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_COLOR_TRANSFORM_HPP_

#include <AvTranscoder/DatasStructures/Pixel.hpp>
#include <AvTranscoder/DatasStructures/Image.hpp>

#include "EssenceTransform.hpp"

#include <vector>
#include <string>

class SwsContext;

namespace avtranscoder
{

class AvExport ColorTransform : public EssenceTransform
{
public:
	ColorTransform();

	void convert( const Frame& src, Frame& dst );

private:
	bool init( const Frame& src, const Frame& dst );

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
