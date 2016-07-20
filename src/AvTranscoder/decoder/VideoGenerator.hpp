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
    VideoGenerator();
    ~VideoGenerator();

    bool decodeNextFrame(Frame& frameBuffer);
    bool decodeNextFrame(Frame& frameBuffer, const std::vector<size_t> channelIndexArray);

    /**
     * @brief Force to return this frame when calling the decoding methods.
     * @param inputFrame: should have the same properties as the given frames when decoding.
     */
    void setNextFrame(Frame& inputFrame) { _inputFrame = &inputFrame; }

private:
    Frame* _inputFrame;              ///< A frame given from outside (has link, no ownership)
    VideoFrame* _blackImage;         ///< The generated RGB black image (has ownership)
    VideoTransform _videoTransform;  ///< To transform data of the back image to the given Frame when decoding.
};
}

#endif
