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

VideoFrameDesc::VideoFrameDesc(const size_t width, const size_t height, const AVPixelFormat pixelFormat)
    : _width(width)
    , _height(height)
    , _pixelFormat(pixelFormat)
    , _fps(1.0)
{
}

VideoFrameDesc::VideoFrameDesc(const size_t width, const size_t height, const std::string& pixelFormatName)
    : _width(width)
    , _height(height)
    , _pixelFormat(getAVPixelFormat(pixelFormatName))
    , _fps(1.0)
{
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
    : Frame()
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
    if(_dataAllocated)
        freeData();
}

size_t VideoFrame::getSize() const
{
    if(getPixelFormat() == AV_PIX_FMT_NONE)
    {
        LOG_WARN("Incorrect pixel format when get size of video frame: return a size of 0.")
        return 0;
    }

    const size_t size = avpicture_get_size(getPixelFormat(), getWidth(), getHeight());
    if(size == 0)
        throw std::runtime_error("unable to determine image buffer size");
    return size;
}

void VideoFrame::allocateData()
{
    // Set Frame properties
    _frame->width = _desc._width;
    _frame->height = _desc._height;
    _frame->format = _desc._pixelFormat;

    // Allocate data
    const int ret = avpicture_alloc(reinterpret_cast<AVPicture*>(_frame), _desc._pixelFormat, _desc._width, _desc._height);
    if(ret < 0)
    {
        std::stringstream os;
        os << "Unable to allocate an image frame of ";
        os << "width = " << _frame->width << ", ";
        os << "height = " << _frame->height << ", ";
        os << "pixel format = " << getPixelFormatName(_desc._pixelFormat);
        LOG_ERROR(os.str())
        throw std::bad_alloc();
    }
    _dataAllocated = true;
}

void VideoFrame::freeData()
{
    avpicture_free(reinterpret_cast<AVPicture*>(_frame));
    _dataAllocated = false;
}

void VideoFrame::assign(const unsigned char value)
{
    // Create the image buffer
    // The buffer will be freed in destructor of based class
    const int imageSize = getSize();
    unsigned char* imageBuffer = new unsigned char[imageSize];
    memset(imageBuffer, value, imageSize);

    // Fill the picture
    assign(imageBuffer);
}

void VideoFrame::assign(const unsigned char* ptrValue)
{
    const int ret =
        avpicture_fill(reinterpret_cast<AVPicture*>(_frame), ptrValue, getPixelFormat(), getWidth(), getHeight());
    if(ret < 0)
    {
        std::stringstream os;
        os << "Unable to assign an image buffer of " << getSize() << " bytes: " << getDescriptionFromErrorCode(ret);
        throw std::runtime_error(os.str());
    }
}
}
