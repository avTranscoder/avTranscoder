#ifndef _MEDIA_ENGINE_TRANSFORM_VIDEO_TRANSFORM_HPP
#define _MEDIA_ENGINE_TRANSFORM_VIDEO_TRANSFORM_HPP

#include "ITransform.hpp"

#include <mediaCore/frame/Frame.hpp>

#include <vector>

class SwsContext;

namespace mediaengine
{
namespace mediatransform
{

class MediaEngineExport VideoTransform : public ITransform
{
public:
	VideoTransform();

	void convert( const mediacore::Frame& srcFrame, mediacore::Frame& dstFrame );

	void setVerbose( bool verbose = false ){ _verbose = verbose; }

private:
	bool init( const mediacore::Frame& srcFrame, const mediacore::Frame& dstFrame );

	SwsContext* _imageConvertContext;

	std::vector<uint8_t *> _srcData;
	std::vector<uint8_t *> _dstData;
	std::vector<int>       _srcLineSize;
	std::vector<int>       _dstLineSize;
	std::vector<size_t>    _srcOffsets;
	std::vector<size_t>    _dstOffsets;

	bool _isInit;

	bool _verbose;
};

}
}

#endif
