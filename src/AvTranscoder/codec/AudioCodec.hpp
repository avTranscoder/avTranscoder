#ifndef _AV_TRANSCODER_CODEC_AUDIO_CODEC_HPP_
#define _AV_TRANSCODER_CODEC_AUDIO_CODEC_HPP_

#include "ICodec.hpp"
#include <AvTranscoder/data/decoded/AudioFrame.hpp>

namespace avtranscoder
{

class AvExport AudioCodec : public ICodec
{
public:
    AudioCodec(const ECodecType type, const std::string& codecName = "");
    AudioCodec(const ECodecType type, const AVCodecID codecId);
    AudioCodec(const ECodecType type, AVCodecContext& avCodecContext);

    AudioFrameDesc getAudioFrameDesc() const;

    void setAudioParameters(const AudioFrameDesc& audioFrameDesc);
};
}

#endif
