#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP

#include "ITransform.hpp"

#include <AvTranscoder/data/decoded/AudioFrame.hpp>

#ifdef AVTRANSCODER_LIBAV_DEPENDENCY
#define ResampleContext AVAudioResampleContext
#else
#define ResampleContext SwrContext
#endif

struct ResampleContext;

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

    void convert(const IFrame& srcFrame, IFrame& dstFrame);

private:
    bool init(const AudioFrame& src, const AudioFrame& dst);

private:
    ResampleContext* _audioConvertContext;

    bool _isInit;
};
}

#endif
