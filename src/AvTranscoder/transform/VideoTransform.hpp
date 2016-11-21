#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_VIDEO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_VIDEO_ESSENCE_TRANSFORM_HPP

#include "ITransform.hpp"

#include <AvTranscoder/data/decoded/VideoFrame.hpp>

struct SwsContext;

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

    void convert(const IFrame& srcFrame, IFrame& dstFrame);

private:
    bool init(const VideoFrame& src, const VideoFrame& dst);

    SwsContext* _imageConvertContext;
    bool _isInit;
};
}

#endif
