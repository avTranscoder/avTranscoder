#include "IFrame.hpp"

#include <stdexcept>

namespace avtranscoder
{

IFrame::IFrame()
    : _frame(NULL)
    , _dataAllocated(false)
{
    allocateAVFrame();
}

IFrame::~IFrame()
{
    freeAVFrame();
}

void IFrame::copyData(const IFrame& frameToRef)
{
    const int ret = av_frame_copy(_frame, &frameToRef.getAVFrame());
    if(ret < 0)
    {
        throw std::ios_base::failure("Unable to copy data of other frame: " + getDescriptionFromErrorCode(ret));
    }
}

void IFrame::copyProperties(const IFrame& otherFrame)
{
    av_frame_copy_props(_frame, &otherFrame.getAVFrame());
}

void IFrame::allocateAVFrame()
{
#if LIBAVCODEC_VERSION_MAJOR > 54
    _frame = av_frame_alloc();
#else
    _frame = avcodec_alloc_frame();
#endif
    if(_frame == NULL)
    {
        LOG_ERROR("Unable to allocate an empty Frame.")
        throw std::bad_alloc();
    }
}

void IFrame::freeAVFrame()
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

void IFrame::assignValue(const unsigned char value)
{
    // Create the buffer
    const int bufferSize = getSize();
    unsigned char* dataBuffer = new unsigned char[bufferSize];
    memset(dataBuffer, value, bufferSize);

    // Fill the frame
    assignBuffer(dataBuffer);
}

bool IFrame::isAudioFrame() const
{
    if(_frame->sample_rate && _frame->channels && _frame->channel_layout && _frame->format != -1)
        return true;
    return false;
}

bool IFrame::isVideoFrame() const
{
    if(_frame->width && _frame->height && _frame->format != -1)
        return true;
    return false;
}
}
