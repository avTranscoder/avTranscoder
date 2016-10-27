#include "Frame.hpp"

#include <stdexcept>

namespace avtranscoder
{

Frame::Frame()
    : _frame(NULL)
{
    allocateAVFrame();
}

Frame::Frame(const Frame& otherFrame)
    : _frame(NULL)
{
    allocateAVFrame();
    copyProperties(otherFrame);
    copyData(otherFrame);
}

void Frame::operator=(const Frame& otherFrame)
{
    allocateAVFrame();
    copyProperties(otherFrame);
    copyData(otherFrame);
}

Frame::~Frame()
{
    freeAVFrame();
}

int Frame::getEncodedSize() const
{
    return av_frame_get_pkt_size(_frame);
}

void Frame::copyData(const Frame& frameToRef)
{
    const int ret = av_frame_copy(_frame, &frameToRef.getAVFrame());
    if(ret < 0)
    {
        throw std::ios_base::failure("Unable to copy data of other frame: " + getDescriptionFromErrorCode(ret));
    }
}

void Frame::copyProperties(const Frame& otherFrame)
{
    av_frame_copy_props(_frame, &otherFrame.getAVFrame());
}

void Frame::allocateAVFrame()
{
#if LIBAVCODEC_VERSION_MAJOR > 54
    _frame = av_frame_alloc();
#else
    _frame = avcodec_alloc_frame();
#endif
    if(_frame == NULL)
    {
        throw std::runtime_error("Unable to allocate an empty Frame.");
    }
}

void Frame::freeAVFrame()
{
    if(_frame != NULL)
    {
#if LIBAVCODEC_VERSION_MAJOR > 54
        av_frame_free(&_frame);
#else
#if LIBAVCODEC_VERSION_MAJOR > 53
        avcodec_free_frame(&_frame);
#else
        av_free(_frame);
#endif
#endif
        _frame = NULL;
    }
}

bool Frame::isAudioFrame() const
{
    if(_frame->sample_rate && _frame->channels && _frame->channel_layout && _frame->format != -1)
        return true;
    return false;
}

bool Frame::isVideoFrame() const
{
    if(_frame->width && _frame->height && _frame->format != -1)
        return true;
    return false;
}
}
