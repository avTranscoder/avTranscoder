#ifndef _AV_TRANSCODER_FRAME_VIDEO_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_VIDEO_FRAME_HPP_

#include "Frame.hpp"
#include <AvTranscoder/profile/ProfileLoader.hpp>

extern "C" {
#include <libavutil/pixfmt.h>
}

namespace avtranscoder
{

class AvExport VideoFrameDesc
{
public:
    VideoFrameDesc(const size_t width = 0, const size_t height = 0, const AVPixelFormat pixelFormat = AV_PIX_FMT_NONE);
    VideoFrameDesc(const size_t width, const size_t height, const std::string& pixelFormat);

    size_t getWidth() const { return _width; }
    size_t getHeight() const { return _height; }
    AVPixelFormat getPixelFormat() const { return _pixelFormat; }
    std::string getPixelFormatName() const;
    double getFps() const { return _fps; }

    size_t getDataSize() const;

    void setWidth(const size_t width) { _width = width; }
    void setHeight(const size_t height) { _height = height; }
    void setPixelFormat(const std::string& pixelFormat);
    void setPixelFormat(const AVPixelFormat pixelFormat) { _pixelFormat = pixelFormat; }
    void setFps(const double fps) { _fps = fps; }

    void setParameters(const ProfileLoader::Profile& profile);

private:
    size_t _width;
    size_t _height;
    AVPixelFormat _pixelFormat;
    double _fps;
};

// template< template<typename> Alloc >
// class AvExport ImageBase
class AvExport VideoFrame : public Frame
{
public:
    VideoFrame(const VideoFrameDesc& ref)
        : Frame(ref.getDataSize())
        , _videoFrameDesc(ref)
    {
    }

    const VideoFrameDesc& desc() const { return _videoFrameDesc; }

private:
    const VideoFrameDesc _videoFrameDesc;
};

// typedef ImageBase<std::allocator> VideoFrame;
}

#endif
