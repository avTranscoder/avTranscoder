#ifndef _AV_TRANSCODER_DECODER_VIDEO_GENERATOR_HPP_
#define _AV_TRANSCODER_DECODER_VIDEO_GENERATOR_HPP_

#include "IDecoder.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>

namespace avtranscoder
{

class AvExport VideoGenerator : public IDecoder
{
private:
    VideoGenerator(const VideoGenerator& videoGenerator);
    VideoGenerator& operator=(const VideoGenerator& videoGenerator);

public:
    VideoGenerator(const VideoFrameDesc& frameDesc);

    ~VideoGenerator();

    bool decodeNextFrame(Frame& frameBuffer);
    bool decodeNextFrame(Frame& frameBuffer, const std::vector<size_t> channelsIndex);

    void setNextFrame(Frame& inputFrame) { _inputFrame = &inputFrame; }

private:
    Frame* _inputFrame;        ///< A frame given from outside (has link, no ownership)
    VideoFrame* _blackImage;   ///< The generated black image (has ownership)
    const VideoFrameDesc _frameDesc; ///< The description of the black image (width, height...)
};
}

#endif
