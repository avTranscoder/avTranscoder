#include "AudioEncoder.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <stdexcept>
#include <libavcodec/avcodec.h>

namespace avtranscoder
{

AudioEncoder::AudioEncoder(const std::string& audioCodecName)
    : _codec(eCodecTypeEncoder, audioCodecName)
{
}

AudioEncoder::~AudioEncoder()
{
}

void AudioEncoder::setupAudioEncoder(const AudioFrameDesc& frameDesc, const ProfileLoader::Profile& profile)
{
    if(!profile.empty())
    {
        LOG_INFO("Setup audio encoder with:\n" << profile)
    }

    // set sampleRate, number of channels, sample format
    _codec.setAudioParameters(frameDesc);

    // setup encoder
    setupEncoder(profile);
}

void AudioEncoder::setupEncoder(const ProfileLoader::Profile& profile)
{
    // check the given profile
    const bool isValid = ProfileLoader::checkAudioProfile(profile);
    if(!isValid && !profile.empty())
    {
        const std::string msg("Invalid audio profile to setup encoder.");
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
           (*it).first == constants::avProfileSampleFormat || (*it).first == constants::avProfileThreads)
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
    _codec.openCodec();

    for(ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it)
    {
        if((*it).first == constants::avProfileIdentificator || (*it).first == constants::avProfileIdentificatorHuman ||
           (*it).first == constants::avProfileType || (*it).first == constants::avProfileCodec ||
           (*it).first == constants::avProfileSampleFormat || (*it).first == constants::avProfileThreads)
            continue;

        try
        {
            Option& encodeOption = _codec.getOption((*it).first);
            encodeOption.setString((*it).second);
        }
        catch(std::exception& e)
        {
            LOG_WARN("AudioEncoder - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what())
        }
    }
}

bool AudioEncoder::encodeFrame(const IFrame& sourceFrame, CodedData& codedFrame)
{
    AVPacket& packet = codedFrame.getAVPacket();
    const AVFrame& srcAvFrame = sourceFrame.getAVFrame();
    if(srcAvFrame.pts != (int)AV_NOPTS_VALUE)
    {
        packet.pts = srcAvFrame.pts;
    }

    if(srcAvFrame.key_frame)
    {
        packet.flags |= AV_PKT_FLAG_KEY;
    }

    return encode(&srcAvFrame, packet);
}

bool AudioEncoder::encodeFrame(CodedData& codedFrame)
{
    return encode(NULL, codedFrame.getAVPacket());
}

bool AudioEncoder::encode(const AVFrame* decodedData, AVPacket& encodedData)
{
    // Be sure that data of AVPacket is NULL so that the encoder will allocate it
    encodedData.data = NULL;

    AVCodecContext& avCodecContext = _codec.getAVCodecContext();
#if LIBAVCODEC_VERSION_MAJOR > 58
    int ret = avcodec_send_frame(&avCodecContext, decodedData);

    if(ret != 0)
        throw std::runtime_error("Error sending audio frame to encoder: " + getDescriptionFromErrorCode(ret));

    ret = avcodec_receive_packet(&avCodecContext, &encodedData);

    if (ret == 0)
        return true;

    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return false;

    throw std::runtime_error("Error receiving audio frame from encoder: " + getDescriptionFromErrorCode(ret));

#elif LIBAVCODEC_VERSION_MAJOR > 53
    int gotPacket = 0;
    const int ret = avcodec_encode_audio2(&avCodecContext, &encodedData, decodedData, &gotPacket);
    if(ret != 0)
    {
        throw std::runtime_error("Encode audio frame error: avcodec encode audio frame - " +
                                 getDescriptionFromErrorCode(ret));
    }
    return gotPacket == 1;
#else
    const int ret = avcodec_encode_audio(&avCodecContext, encodedData.data, encodedData.size, decodedData);
    if(ret < 0)
    {
        throw std::runtime_error("Encode audio frame error: avcodec encode audio frame - " +
                                 getDescriptionFromErrorCode(ret));
    }
    return true;
#endif
}
}
