#include "VideoEncoder.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <stdexcept>
#include <cstdlib>

namespace avtranscoder
{

VideoEncoder::VideoEncoder(const std::string& videoCodecName)
    : _codec(eCodecTypeEncoder, videoCodecName)
{
}

VideoEncoder::~VideoEncoder()
{
}

void VideoEncoder::setupVideoEncoder(const VideoFrameDesc& frameDesc, const ProfileLoader::Profile& profile)
{
    if(!profile.empty())
    {
        LOG_INFO("Setup video encoder with:\n" << profile)
    }

    // set width, height, pixel format, fps
    _codec.setImageParameters(frameDesc);

    // setup encoder
    setupEncoder(profile);
}

void VideoEncoder::setupEncoder(const ProfileLoader::Profile& profile)
{
    // check the given profile
    const bool isValid = ProfileLoader::checkVideoProfile(profile);
    if(!isValid && !profile.empty())
    {
        const std::string msg("Invalid video profile to setup encoder.");
        LOG_ERROR(msg)
        throw std::runtime_error(msg);
    }

    // set threads before any other options
    if(profile.count(constants::avProfileThreads))
        _codec.getOption(constants::avProfileThreads).setString(profile.at(constants::avProfileThreads));
    else
        _codec.getOption(constants::avProfileThreads).setInt(_codec.getAVCodecContext().thread_count);

    // set encoder options
    for(ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it)
    {
        if((*it).first == constants::avProfileIdentificator || (*it).first == constants::avProfileIdentificatorHuman ||
           (*it).first == constants::avProfileType || (*it).first == constants::avProfileCodec ||
           (*it).first == constants::avProfileWidth || (*it).first == constants::avProfileHeight ||
           (*it).first == constants::avProfilePixelFormat || (*it).first == constants::avProfileFrameRate ||
           (*it).first == constants::avProfileThreads)
            continue;

        try
        {
            Option& encodeOption = _codec.getOption((*it).first);
            encodeOption.setString((*it).second);
        }
        catch(std::exception& e)
        {
        }
    }

    // open encoder
    int encoderFlags = 0;
    if(profile.count(constants::avProfileProcessStat))
    {
        LOG_INFO("SetUp video encoder to compute statistics during process")
        encoderFlags |= CODEC_FLAG_PSNR;
    }
    _codec.getAVCodecContext().flags |= encoderFlags;
    _codec.openCodec();

    // after open encoder, set specific encoder options
    for(ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it)
    {
        if((*it).first == constants::avProfileIdentificator || (*it).first == constants::avProfileIdentificatorHuman ||
           (*it).first == constants::avProfileType || (*it).first == constants::avProfileCodec ||
           (*it).first == constants::avProfileWidth || (*it).first == constants::avProfileHeight ||
           (*it).first == constants::avProfilePixelFormat || (*it).first == constants::avProfileFrameRate ||
           (*it).first == constants::avProfileThreads)
            continue;

        try
        {
            Option& encodeOption = _codec.getOption((*it).first);
            encodeOption.setString((*it).second);
        }
        catch(std::exception& e)
        {
            LOG_WARN("VideoEncoder - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what())
        }
    }
}

bool VideoEncoder::encodeFrame(const Frame& sourceFrame, CodedData& codedFrame)
{
    AVCodecContext& avCodecContext = _codec.getAVCodecContext();

    AVPacket& packet = codedFrame.getAVPacket();
    if((avCodecContext.coded_frame) && (avCodecContext.coded_frame->pts != (int)AV_NOPTS_VALUE))
    {
        packet.pts = avCodecContext.coded_frame->pts;
    }

    if(avCodecContext.coded_frame && avCodecContext.coded_frame->key_frame)
    {
        packet.flags |= AV_PKT_FLAG_KEY;
    }

    return encode(&sourceFrame.getAVFrame(), packet);
}

bool VideoEncoder::encodeFrame(CodedData& codedFrame)
{
    return encode(NULL, codedFrame.getAVPacket());
}

bool VideoEncoder::encode(const AVFrame* decodedData, AVPacket& encodedData)
{
    // Be sure that data of AVPacket is NULL so that the encoder will allocate it
    encodedData.data = NULL;
    encodedData.stream_index = 0;

    AVCodecContext& avCodecContext = _codec.getAVCodecContext();
#if LIBAVCODEC_VERSION_MAJOR > 53
    int gotPacket = 0;
    const int ret = avcodec_encode_video2(&avCodecContext, &encodedData, decodedData, &gotPacket);
    if(ret != 0)
    {
        throw std::runtime_error("Encode video frame error: avcodec encode video frame - " +
                                 getDescriptionFromErrorCode(ret));
    }
    return ret == 0 && gotPacket == 1;
#else
    const int ret = avcodec_encode_video(&avCodecContext, encodedData.data, encodedData.size, decodedData);
    if(ret < 0)
    {
        throw std::runtime_error("Encode video frame error: avcodec encode video frame - " +
                                 getDescriptionFromErrorCode(ret));
    }
    return ret == 0;
#endif
}

}
