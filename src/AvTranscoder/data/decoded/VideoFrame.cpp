#include "VideoFrame.hpp"

#include <AvTranscoder/util.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>
#include <stdlib.h>

namespace avtranscoder
{

VideoFrameDesc::VideoFrameDesc(const size_t width, const size_t height, const std::string& pixelFormatName)
    : _width(width)
    , _height(height)
    , _pixelFormat(getAVPixelFormat(pixelFormatName))
    , _fps(1.0)
{
}

VideoFrameDesc::VideoFrameDesc(const ProfileLoader::Profile& profile)
    : _width(0)
    , _height(0)
    , _pixelFormat(AV_PIX_FMT_NONE)
    , _fps(1.0)
{
    setParameters(profile);
}


void VideoFrameDesc::setParameters(const ProfileLoader::Profile& profile)
{
    // width
    if(profile.count(constants::avProfileWidth))
        _width = atoi(profile.find(constants::avProfileWidth)->second.c_str());
    // height
    if(profile.count(constants::avProfileHeight))
        _height = atoi(profile.find(constants::avProfileHeight)->second.c_str());
    // pixel format
    if(profile.count(constants::avProfilePixelFormat))
        _pixelFormat = getAVPixelFormat(profile.find(constants::avProfilePixelFormat)->second);
    // fps
    if(profile.count(constants::avProfileFrameRate))
        _fps = atof(profile.find(constants::avProfileFrameRate)->second.c_str());
}

VideoFrame::VideoFrame(const VideoFrameDesc& desc, const bool forceDataAllocation)
    : IFrame()
    , _desc(desc)
{
    _frame->width = desc._width;
    _frame->height = desc._height;
    _frame->format = desc._pixelFormat;

    if(forceDataAllocation)
        allocateData();
}

VideoFrame::~VideoFrame()
{
    if(_frame->buf[0])
        av_frame_unref(_frame);
    if(_dataAllocated)
        freeData();
}

size_t VideoFrame::getDataSize() const
{
    if(getPixelFormat() == AV_PIX_FMT_NONE)
    {
        LOG_WARN("Incorrect pixel format when get size of video frame: return a size of 0.")
        return 0;
    }

    const size_t size = avpicture_get_size(getPixelFormat(), getWidth(), getHeight());
    if(size == 0)
        throw std::runtime_error("Unable to determine image buffer size: " + getDescriptionFromErrorCode(size));
    return size;
}

void VideoFrame::allocateData()
{
    if(_dataAllocated)
        LOG_WARN("The VideoFrame seems to already have allocated data. This could lead to memory leaks.")

    // Set Frame properties
    _frame->width = _desc._width;
    _frame->height = _desc._height;
    _frame->format = _desc._pixelFormat;

    // Allocate data
    const int ret = avpicture_alloc(reinterpret_cast<AVPicture*>(_frame), _desc._pixelFormat, _desc._width, _desc._height);
    if(ret < 0)
    {
        const std::string formatName = getPixelFormatName(_desc._pixelFormat);
        std::stringstream msg;
        msg << "Unable to allocate an image frame of ";
        msg << "width = " << _frame->width << ", ";
        msg << "height = " << _frame->height << ", ";
        msg << "pixel format = " << (formatName.empty() ? "none" : formatName);
        LOG_ERROR(msg.str())
        throw std::bad_alloc();
    }
    _dataAllocated = true;
}

void VideoFrame::freeData()
{
    avpicture_free(reinterpret_cast<AVPicture*>(_frame));
    _dataAllocated = false;
}

void VideoFrame::assignBuffer(const unsigned char* ptrValue)
{
    const int ret =
        avpicture_fill(reinterpret_cast<AVPicture*>(_frame), ptrValue, getPixelFormat(), getWidth(), getHeight());
    if(ret < 0)
    {
        std::stringstream msg;
        msg << "Unable to assign an image buffer of " << getDataSize() << " bytes: " << getDescriptionFromErrorCode(ret);
        throw std::runtime_error(msg.str());
    }
}
}
