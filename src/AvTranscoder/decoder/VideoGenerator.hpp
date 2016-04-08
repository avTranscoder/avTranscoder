#ifndef _AV_TRANSCODER_DECODER_VIDEO_GENERATOR_HPP_
#define _AV_TRANSCODER_DECODER_VIDEO_GENERATOR_HPP_

#include "IDecoder.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>

namespace avtranscoder
{

class AvExport VideoGenerator : public IDecoder
{
public:
    VideoGenerator();
    VideoGenerator(const VideoGenerator& videoGenerator);
    VideoGenerator& operator=(const VideoGenerator& videoGenerator);

    ~VideoGenerator();

    bool decodeNextFrame(Frame& frameBuffer);
    bool decodeNextFrame(Frame& frameBuffer, const size_t channelIndex);

    const VideoFrameDesc& getVideoFrameDesc() const { return _frameDesc; }
    void setVideoFrameDesc(const VideoFrameDesc& frameDesc) { _frameDesc = frameDesc; }

    void setNextFrame(Frame& inputFrame) { _inputFrame = &inputFrame; }

private:
    Frame* _inputFrame;        ///< A frame given from outside (has link, no ownership)
    VideoFrame* _blackImage;   ///< The generated black image (has ownership)
    VideoFrameDesc _frameDesc; ///< The description of the black image (width, height...)
};
}

#endif
