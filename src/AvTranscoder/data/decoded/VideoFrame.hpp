#ifndef _AV_TRANSCODER_FRAME_VIDEO_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_VIDEO_FRAME_HPP_

#include "Frame.hpp"
#include <AvTranscoder/profile/ProfileLoader.hpp>

extern "C" {
#include <libavutil/pixfmt.h>
#include <libavutil/pixdesc.h>
}

namespace avtranscoder
{

/**
 * @brief Description to create a video frame.
 * @param width
 * @param height
 * @param pixelFormat
 */
struct AvExport VideoFrameDesc
{
public:
    VideoFrameDesc(const size_t width = 0, const size_t height = 0, const AVPixelFormat pixelFormat = AV_PIX_FMT_NONE);
    VideoFrameDesc(const size_t width, const size_t height, const std::string& pixelFormatName);

    /**
     * @brief Set the attributes from the given profile.
     * @see Profile
     */
    void setParameters(const ProfileLoader::Profile& profile);

public:
    size_t _width;
    size_t _height;
    AVPixelFormat _pixelFormat;
    double _fps;
};

/**
 * @brief This class describes decoded video data.
 */
class AvExport VideoFrame : public Frame
{
public:
    VideoFrame(const VideoFrameDesc& ref);
    VideoFrame(const Frame& otherFrame);

    size_t getWidth() const { return _frame->width; }
    size_t getHeight() const { return _frame->height; }
    AVPixelFormat getPixelFormat() const { return static_cast<AVPixelFormat>(_frame->format); }
    VideoFrameDesc desc() const { return VideoFrameDesc(getWidth(), getHeight(), getPixelFormat()); }

    size_t getSize() const; ///< in bytes/**

    /**
     * @brief Assign the given value to all the data of the picture.
     */
    void assign(const unsigned char value);

    /**
     * @brief Assign the given ptr of data to the data of the picture.
     * @warning the given ptr should have the size of the picture.
     * @see getSize
     */
    void assign(const unsigned char* ptrValue);

private:
    /**
     * @brief Allocate the image buffer of the frame.
     */
    void allocateAVPicture(const VideoFrameDesc& desc);
};
}

#endif
