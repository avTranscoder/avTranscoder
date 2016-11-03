#ifndef _AV_TRANSCODER_DECODER_AUDIO_GENERATOR_HPP_
#define _AV_TRANSCODER_DECODER_AUDIO_GENERATOR_HPP_

#include "IDecoder.hpp"
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/transform/AudioTransform.hpp>

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

    bool decodeNextFrame(IFrame& frameBuffer);
    bool decodeNextFrame(IFrame& frameBuffer, const std::vector<size_t> channelIndexArray);

    /**
     * @brief Force to return this frame when calling the decoding methods.
     * @param inputFrame: could have other properties than the given frame when decoding (will be converted).
     * @see decodeNextFrame
     */
    void setNextFrame(IFrame& inputFrame) { _inputFrame = &inputFrame; }

private:
    IFrame* _inputFrame;              ///< Has link (no ownership)
    AudioFrame* _silent;             ///< The generated silent (has ownership)
    const AudioFrameDesc _frameDesc; ///< The description of the given frame buffer when decoding.
    AudioTransform _audioTransform;  ///< To transform the specified data when decoding.
};
}

#endif
