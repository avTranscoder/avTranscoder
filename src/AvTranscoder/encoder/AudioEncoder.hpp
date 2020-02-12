#ifndef _AV_TRANSCODER_ENCODER_AUDIO_ENCODER_HPP_
#define _AV_TRANSCODER_ENCODER_AUDIO_ENCODER_HPP_

#include <AvTranscoder/encoder/IEncoder.hpp>
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/profile/ProfileLoader.hpp>

namespace avtranscoder
{

class AvExport AudioEncoder : public IEncoder
{
public:
    AudioEncoder(const std::string& audioCodecName);
    ~AudioEncoder();

    void setupAudioEncoder(const AudioFrameDesc& frameDesc,
                           const ProfileLoader::Profile& profile = ProfileLoader::Profile());
    void setupEncoder(const ProfileLoader::Profile& profile = ProfileLoader::Profile());

    bool encodeFrame(const IFrame& sourceFrame, CodedData& codedFrame);
    bool encodeFrame(CodedData& codedFrame);

    ICodec& getCodec() { return _codec; }
    AudioCodec& getAudioCodec() { return _codec; }

private:
    bool encode(const AVFrame* decodedData, AVPacket& encodedData);

private:
    AudioCodec _codec;
};
}

#endif
