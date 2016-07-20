#ifndef _AV_TRANSCODER_DECODER_AUDIO_GENERATOR_HPP_
#define _AV_TRANSCODER_DECODER_AUDIO_GENERATOR_HPP_

#include "IDecoder.hpp"
#include <AvTranscoder/codec/AudioCodec.hpp>

namespace avtranscoder
{

class AvExport AudioGenerator : public IDecoder
{
private:
    AudioGenerator& operator=(const AudioGenerator& audioGenerator);
    AudioGenerator(const AudioGenerator& audioGenerator);

public:
    AudioGenerator(const AudioFrameDesc& frameDesc);

    ~AudioGenerator();

    bool decodeNextFrame(Frame& frameBuffer);
    bool decodeNextFrame(Frame& frameBuffer, const std::vector<size_t> channelIndexArray);

    void setNextFrame(Frame& inputFrame) { _inputFrame = &inputFrame; }

private:
    Frame* _inputFrame;              ///< Has link (no ownership)
    AudioFrame* _silent;             ///< The generated silent (has ownership)
    const AudioFrameDesc _frameDesc; ///< The description of the silence (sampleRate, channels...)
};
}

#endif
