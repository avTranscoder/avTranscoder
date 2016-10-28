#include "VideoTransform.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#if LIBAVCODEC_VERSION_MAJOR > 54
#include <libavutil/frame.h>
#endif
}

#include <sstream>
#include <iomanip>
#include <cassert>
#include <stdexcept>

namespace avtranscoder
{

VideoTransform::VideoTransform()
    : _imageConvertContext(NULL)
    , _isInit(false)
{
}

VideoTransform::~VideoTransform()
{
    sws_freeContext(_imageConvertContext);
}

bool VideoTransform::init(const VideoFrame& src, const VideoFrame& dst)
{
    const AVPixelFormat srcPixelFormat = src.getPixelFormat();
    const AVPixelFormat dstPixelFormat = dst.getPixelFormat();

    _imageConvertContext =
        sws_getCachedContext(_imageConvertContext, src.getWidth(), src.getHeight(), srcPixelFormat, dst.getWidth(),
                             dst.getHeight(), dstPixelFormat, SWS_POINT, NULL, NULL, NULL);

    if(!_imageConvertContext)
    {
        throw std::runtime_error("unable to create color convert context");
    }

    const char* srcPixFmt;
    srcPixFmt = av_get_pix_fmt_name(srcPixelFormat);
    const char* dstPixFmt;
    dstPixFmt = av_get_pix_fmt_name(dstPixelFormat);

    std::stringstream msg;
    msg << "Video conversion from " << (srcPixFmt != NULL ? srcPixFmt : "None") << " to "
        << (dstPixFmt != NULL ? dstPixFmt : "None") << std::endl;
    msg << "Source, width = " << src.getWidth() << std::endl;
    msg << "Source, height = " << src.getHeight() << std::endl;
    msg << "Destination, width = " << dst.getWidth() << std::endl;
    msg << "Destination, height = " << dst.getHeight() << std::endl;
    LOG_INFO(msg.str())

    return true;
}

void VideoTransform::convert(const IFrame& srcFrame, IFrame& dstFrame)
{
    const VideoFrame& src = static_cast<const VideoFrame&>(srcFrame);
    VideoFrame& dst = static_cast<VideoFrame&>(dstFrame);

    assert(src.getWidth() > 0);
    assert(src.getHeight() > 0);
    assert(src.getPixelFormat() != AV_PIX_FMT_NONE);
    assert(dst.getSize() > 0);

    if(!_isInit)
        _isInit = init(src, dst);

    if(!_imageConvertContext)
    {
        throw std::runtime_error("unknown color convert context");
    }

    // Convert
    const int ret = sws_scale(_imageConvertContext, src.getData(), src.getLineSize(), 0, src.getHeight(), dst.getData(),
                              dst.getLineSize());

    if(ret != (int)dst.getHeight())
        throw std::runtime_error("error in color converter");
}
}
