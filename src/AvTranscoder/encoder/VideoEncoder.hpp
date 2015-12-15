#ifndef _AV_TRANSCODER_ENCODER_VIDEO_ENCODER_HPP_
#define _AV_TRANSCODER_ENCODER_VIDEO_ENCODER_HPP_

#include "IEncoder.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/profile/ProfileLoader.hpp>

namespace avtranscoder
{

class AvExport VideoEncoder : public IEncoder
{
public:
    VideoEncoder(const std::string& videoCodecName);
    ~VideoEncoder();

    void setupVideoEncoder(const VideoFrameDesc& frameDesc,
                           const ProfileLoader::Profile& profile = ProfileLoader::Profile());
    void setupEncoder(const ProfileLoader::Profile& profile = ProfileLoader::Profile());

    bool encodeFrame(const Frame& sourceFrame, Frame& codedFrame);
    bool encodeFrame(Frame& codedFrame);

    ICodec& getCodec() { return _codec; }
    VideoCodec& getVideoCodec() { return _codec; }

private:
    VideoCodec _codec;
    AVFrame* _frame; ///< Contains the encoded data to pass to the Frame when encodeFrame
};
}

#endif