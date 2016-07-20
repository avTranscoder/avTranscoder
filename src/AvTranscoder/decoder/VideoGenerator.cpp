#include "VideoGenerator.hpp"

#include <AvTranscoder/util.hpp>

#include <sstream>
#include <stdexcept>

namespace avtranscoder
{

VideoGenerator::VideoGenerator()
    : _inputFrame(NULL)
    , _blackImage(NULL)
    , _videoTransform()
{
}

VideoGenerator::~VideoGenerator()
{
    delete _blackImage;
}

bool VideoGenerator::decodeNextFrame(Frame& frameBuffer)
{
    // check the given frame
    if(! frameBuffer.isVideoFrame())
        throw std::runtime_error("The given frame is not a valid video frame: allocate a new AVPicture to put generated data into it.");

    // Generate black image
    if(!_inputFrame)
    {
        // Generate the black image only once
        if(!_blackImage)
        {
            std::stringstream msg;
            msg << "Generate a black image with the following features:" << std::endl;
            msg << "width = " << frameBuffer.getAVFrame().width << std::endl;
            msg << "height = " << frameBuffer.getAVFrame().height << std::endl;
            msg << "pixel format = rgb24" << std::endl;
            LOG_INFO(msg.str())

            // Create the black RGB image
            _blackImage = new VideoFrame(VideoFrameDesc(frameBuffer.getAVFrame().width, frameBuffer.getAVFrame().height, "rgb24"));
            const unsigned char fillChar = 0;
            _blackImage->assign(fillChar);
        }
        LOG_DEBUG("Copy data of the black image when decode next frame")
        // Convert the black image to the configuration of the given frame
        _videoTransform.convert(*_blackImage, frameBuffer);
    }
    // Take image from _inputFrame
    else
    {
        LOG_DEBUG("Copy data of the image specified when decode next frame")
        frameBuffer.copyData(*_inputFrame);
    }
    return true;
}

bool VideoGenerator::decodeNextFrame(Frame& frameBuffer, const std::vector<size_t> channelIndexArray)
{
    return false;
}
}
