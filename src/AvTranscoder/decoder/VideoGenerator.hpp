#ifndef _AV_TRANSCODER_DECODER_VIDEO_GENERATOR_HPP_
#define _AV_TRANSCODER_DECODER_VIDEO_GENERATOR_HPP_

#include "IDecoder.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>

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
    bool decodeNextFrame(Frame& frameBuffer, const std::vector<size_t> channelIndexArray);

    /**
     * @brief Force to return this frame when calling the decoding methods.
     * @param inputFrame: could have other properties than the given frame when decoding (will be converted).
     * @see decodeNextFrame
     */
    void setNextFrame(Frame& inputFrame) { _inputFrame = &inputFrame; }

private:
    Frame* _inputFrame;              ///< A frame given from outside (has link, no ownership)
    VideoFrame* _blackImage;         ///< The generated RGB black image (has ownership)
    const VideoFrameDesc _frameDesc; ///< The description of the given frame buffer when decoding.
    VideoTransform _videoTransform;  ///< To transform data of the back image to the given Frame when decoding.
};
}

#endif
