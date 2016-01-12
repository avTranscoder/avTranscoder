#include "VideoTransform.hpp"

#include <AvTranscoder/data/VideoFrame.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#if LIBAVCODEC_VERSION_MAJOR > 54
#include <libavutil/frame.h>
#endif
}

#define MAX_SWS_PLANE 4

#include <sstream>
#include <iomanip>
#include <cassert>
#include <stdexcept>

namespace avtranscoder
{

VideoTransform::VideoTransform()
    : _imageConvertContext(NULL)
    , _srcData((uint8_t)MAX_SWS_PLANE, NULL)
    , _dstData((uint8_t)MAX_SWS_PLANE, NULL)
    , _srcLineSize(MAX_SWS_PLANE, 0)
    , _dstLineSize(MAX_SWS_PLANE, 0)
    , _isInit(false)
{
}

VideoTransform::~VideoTransform()
{
    sws_freeContext(_imageConvertContext);
}

bool VideoTransform::init(const Frame& srcFrame, const Frame& dstFrame)
{
    const VideoFrame& src = static_cast<const VideoFrame&>(srcFrame);
    const VideoFrame& dst = static_cast<const VideoFrame&>(dstFrame);

    const AVPixelFormat srcPixelFormat = src.desc().getPixelFormat();
    const AVPixelFormat dstPixelFormat = dst.desc().getPixelFormat();

    _imageConvertContext =
        sws_getCachedContext(_imageConvertContext, src.desc().getWidth(), src.desc().getHeight(), srcPixelFormat,
                             dst.desc().getWidth(), dst.desc().getHeight(), dstPixelFormat, SWS_POINT, NULL, NULL, NULL);

    if(!_imageConvertContext)
    {
        throw std::runtime_error("unable to create color convert context");
    }

    av_image_fill_linesizes(&_srcLineSize[0], srcPixelFormat, src.desc().getWidth());
    av_image_fill_linesizes(&_dstLineSize[0], dstPixelFormat, dst.desc().getWidth());

    const char* srcPixFmt;
    srcPixFmt = av_get_pix_fmt_name(srcPixelFormat);
    const char* dstPixFmt;
    dstPixFmt = av_get_pix_fmt_name(dstPixelFormat);

    std::stringstream msg;
    msg << "Video conversion from " << (srcPixFmt != NULL ? srcPixFmt : "None") << " to "
        << (dstPixFmt != NULL ? dstPixFmt : "None") << std::endl;
    msg << "Source, width = " << src.desc().getWidth() << std::endl;
    msg << "Source, height = " << src.desc().getHeight() << std::endl;
    msg << "Destination, width = " << dst.desc().getWidth() << std::endl;
    msg << "Destination, height = " << dst.desc().getHeight() << std::endl;
    LOG_INFO(msg.str())

    return true;
}

void VideoTransform::convert(const Frame& srcFrame, Frame& dstFrame)
{
    const VideoFrame& src = static_cast<const VideoFrame&>(srcFrame);
    VideoFrame& dst = static_cast<VideoFrame&>(dstFrame);

    assert(src.desc().getWidth() != 0);
    assert(src.desc().getHeight() != 0);
    assert(src.desc().getPixelFormat() != AV_PIX_FMT_NONE);

    if(!_isInit)
        _isInit = init(srcFrame, dstFrame);

    const AVPixelFormat srcPixelFormat = src.desc().getPixelFormat();
    const AVPixelFormat dstPixelFormat = dst.desc().getPixelFormat();

    // Fill plane data pointers
    av_image_fill_pointers(&_srcData[0], srcPixelFormat, src.desc().getHeight(), (uint8_t*)src.getData(), &_srcLineSize[0]);
    av_image_fill_pointers(&_dstData[0], dstPixelFormat, dst.desc().getHeight(), (uint8_t*)dst.getData(), &_dstLineSize[0]);

    if(!_imageConvertContext)
    {
        throw std::runtime_error("unknown color convert context");
    }

    int ret = sws_scale(_imageConvertContext, &_srcData[0], &_srcLineSize[0], 0, src.desc().getHeight(), &_dstData[0],
                        &_dstLineSize[0]);

    if(ret != (int)dst.desc().getHeight())
        throw std::runtime_error("error in color converter");
}
}
