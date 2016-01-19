#include "AudioCodec.hpp"

#include <cmath>
#include <cassert>

namespace avtranscoder
{

AudioCodec::AudioCodec(const ECodecType type, const std::string& codecName)
    : ICodec(type, codecName)
{
}

AudioCodec::AudioCodec(const ECodecType type, const AVCodecID codecId)
    : ICodec(type, codecId)
{
}

AudioCodec::AudioCodec(const ECodecType type, AVCodecContext& avCodecContext)
    : ICodec(type, avCodecContext)
{
}

AudioFrameDesc AudioCodec::getAudioFrameDesc() const
{
    assert(_avCodecContext != NULL);
    return AudioFrameDesc(_avCodecContext->sample_rate, _avCodecContext->channels, _avCodecContext->sample_fmt);
}

void AudioCodec::setAudioParameters(const AudioFrameDesc& audioFrameDesc)
{
    _avCodecContext->sample_rate = audioFrameDesc._sampleRate;
    _avCodecContext->channels = audioFrameDesc._nbChannels;
    _avCodecContext->sample_fmt = audioFrameDesc._sampleFormat;
}
}
