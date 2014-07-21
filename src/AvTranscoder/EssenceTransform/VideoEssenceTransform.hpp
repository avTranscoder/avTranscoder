#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_VIDEO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_VIDEO_ESSENCE_TRANSFORM_HPP

#include <AvTranscoder/EssenceStructures/Frame.hpp>
#include "EssenceTransform.hpp"

#include <vector>

class SwsContext;

namespace avtranscoder
{

class AvExport VideoEssenceTransform : public EssenceTransform
{
public:
	VideoEssenceTransform();

	void convert( const Frame& srcFrame, Frame& dstFrame );

private:
	bool init( const Frame& srcFrame, const Frame& dstFrame );

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
