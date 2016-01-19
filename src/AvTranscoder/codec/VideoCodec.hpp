#ifndef _AV_TRANSCODER_CODEC_VIDEO_CODEC_HPP_
#define _AV_TRANSCODER_CODEC_VIDEO_CODEC_HPP_

#include "ICodec.hpp"
#include <AvTranscoder/data/decoded/VideoFrame.hpp>

namespace avtranscoder
{

class AvExport VideoCodec : public ICodec
{
public:
    VideoCodec(const ECodecType type, const std::string& codecName = "");
    VideoCodec(const ECodecType type, const AVCodecID codecId);
    VideoCodec(const ECodecType type, AVCodecContext& avCodecContext);

    VideoFrameDesc getVideoFrameDesc() const;

    void setImageParameters(const VideoFrameDesc& videoFrameDesc);
};
}

#endif