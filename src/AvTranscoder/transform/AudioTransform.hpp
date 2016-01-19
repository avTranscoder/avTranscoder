#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP

#include "ITransform.hpp"

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/data/decoded/Frame.hpp>

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

private:
    ResampleContext* _audioConvertContext;

    bool _isInit;
};
}

#endif
