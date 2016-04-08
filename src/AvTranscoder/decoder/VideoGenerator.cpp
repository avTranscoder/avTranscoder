#include "VideoGenerator.hpp"

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>

#include <sstream>

namespace avtranscoder
{

VideoGenerator::VideoGenerator(const VideoFrameDesc& frameDesc)
    : _inputFrame(NULL)
    , _blackImage(NULL)
    , _frameDesc(frameDesc)
{
}

VideoGenerator::VideoGenerator(const VideoGenerator& videoGenerator)
    : _inputFrame(NULL)
    , _blackImage(NULL)
    , _frameDesc(videoGenerator._frameDesc)
{
}

VideoGenerator& VideoGenerator::operator=(const VideoGenerator& videoGenerator)
{
    _inputFrame = NULL;
    _blackImage = NULL;
    _frameDesc = videoGenerator._frameDesc;
    return *this;
}

VideoGenerator::~VideoGenerator()
{
    delete _blackImage;
}

bool VideoGenerator::decodeNextFrame(Frame& frameBuffer)
{
    // check the given frame
    if(!frameBuffer.isVideoFrame())
    {
        LOG_WARN("The given frame is not a valid video frame: allocate a new AVPicture to put generated data into it.");
        frameBuffer.clear();
        static_cast<VideoFrame&>(frameBuffer).allocateAVPicture(_frameDesc);
    }

    // Generate black image
    if(!_inputFrame)
    {
        // Generate the black image only once
        if(!_blackImage)
        {
            std::stringstream msg;
            msg << "Generate a black image with the following features:" << std::endl;
            msg << "width = " << _frameDesc._width << std::endl;
            msg << "height = " << _frameDesc._height << std::endl;
            msg << "pixel format = rgb24" << std::endl;
            LOG_INFO(msg.str())

            VideoFrame& imageBuffer = static_cast<VideoFrame&>(frameBuffer);

            // Input of convert
            // @todo support PAL (0 to 255) and NTFS (16 to 235)
            VideoFrameDesc desc(_frameDesc);
            desc._pixelFormat = getAVPixelFormat("rgb24");
            VideoFrame intermediateBuffer(desc);
            const unsigned char fillChar = 0;
            intermediateBuffer.assign(fillChar);

            // Output of convert
            _blackImage = new VideoFrame(imageBuffer.desc());

            // Convert and store the black image
            VideoTransform videoTransform;
            videoTransform.convert(intermediateBuffer, *_blackImage);
        }
        LOG_DEBUG("Copy data of the black image when decode next frame")
        frameBuffer.copyData(*_blackImage);
    }
    // Take image from _inputFrame
    else
    {
        LOG_DEBUG("Copy data of the image specified when decode next frame")
        frameBuffer.copyData(*_inputFrame);
    }
    return true;
}

bool VideoGenerator::decodeNextFrame(Frame& frameBuffer, const size_t channelIndex)
{
    return false;
}
}
