#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP

#include "ITransform.hpp"

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/frame/Frame.hpp>

#ifdef AVTRANSCODER_LIBAV_DEPENDENCY
#define ResampleContext AVAudioResampleContext
#else
#define ResampleContext SwrContext
#endif

class ResampleContext;

namespace avtranscoder
{

class AvExport AudioTransform : public ITransform
{
private:
    AudioTransform(const AudioTransform& audioTransform);
    AudioTransform& operator=(const AudioTransform& audioTransform);

public:
    AudioTransform();
    ~AudioTransform();

    void convert(const Frame& srcFrame, Frame& dstFrame);

private:
    bool init(const Frame& srcFrame, const Frame& dstFrame);

    /// Update output buffer if source has a different size from the last process
    void updateOutputFrame(const size_t nbInputSamples, Frame& dstFrame) const;

private:
    ResampleContext* _audioConvertContext;

    size_t _previousNbInputSamplesPerChannel; ///< To check if the number of samples change between frames

    bool _isInit;
};
}

#endif
