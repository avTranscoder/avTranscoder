#include "VideoGenerator.hpp"

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>

namespace avtranscoder
{

VideoGenerator::VideoGenerator()
    : _inputFrame(NULL)
    , _blackImage(NULL)
    , _frameDesc()
{
}

VideoGenerator::VideoGenerator(const VideoGenerator& videoGenerator)
    : _inputFrame(NULL)
    , _blackImage(NULL)
    , _frameDesc(videoGenerator.getVideoFrameDesc())
{
}

VideoGenerator& VideoGenerator::operator=(const VideoGenerator& videoGenerator)
{
    _inputFrame = NULL;
    _blackImage = NULL;
    _frameDesc = videoGenerator.getVideoFrameDesc();
    return *this;
}

VideoGenerator::~VideoGenerator()
{
    delete _blackImage;
}

void VideoGenerator::setVideoFrameDesc(const VideoFrameDesc& frameDesc)
{
    _frameDesc = frameDesc;
}

void VideoGenerator::setNextFrame(Frame& inputFrame)
{
    _inputFrame = &inputFrame;
}

bool VideoGenerator::decodeNextFrame(Frame& frameBuffer)
{
    // Generate black image
    if(!_inputFrame)
    {
        // Generate the black image only once
        if(!_blackImage)
        {
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
        frameBuffer.refData(*_blackImage);
    }
    // Take image from _inputFrame
    else
    {
        frameBuffer.refData(*_inputFrame);
    }
    return true;
}

bool VideoGenerator::decodeNextFrame(Frame& frameBuffer, const size_t channelIndex)
{
    return false;
}
}
