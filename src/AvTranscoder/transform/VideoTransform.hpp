#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_VIDEO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_VIDEO_ESSENCE_TRANSFORM_HPP

#include <vector>

#include "ITransform.hpp"

#include <AvTranscoder/data/decoded/Frame.hpp>

class SwsContext;

namespace avtranscoder
{

class AvExport VideoTransform : public ITransform
{
private:
    VideoTransform(const VideoTransform& videoTransform);
    VideoTransform& operator=(const VideoTransform& videoTransform);

public:
    VideoTransform();
    ~VideoTransform();

    void convert(const Frame& srcFrame, Frame& dstFrame);

private:
    bool init(const Frame& srcFrame, const Frame& dstFrame);

    SwsContext* _imageConvertContext;
    bool _isInit;
};
}

#endif
