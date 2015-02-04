#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_VIDEO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_VIDEO_ESSENCE_TRANSFORM_HPP

#include <vector>

#include "ITransform.hpp"

#include <AvTranscoder/frame/Frame.hpp>

class SwsContext;

namespace avtranscoder
{

class AvExport VideoTransform : public ITransform
{
public:
	VideoTransform();
	~VideoTransform();

	void convert( const Frame& srcFrame, Frame& dstFrame );

	void setVerbose( bool verbose = false ){ _verbose = verbose; }

private:
	bool init( const Frame& srcFrame, const Frame& dstFrame );

	SwsContext* _imageConvertContext;

	std::vector<uint8_t *> _srcData;
	std::vector<uint8_t *> _dstData;
	std::vector<int>       _srcLineSize;
	std::vector<int>       _dstLineSize;

	bool _isInit;

	bool _verbose;
};

}

#endif
