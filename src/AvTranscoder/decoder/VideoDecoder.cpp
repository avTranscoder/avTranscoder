#include "VideoDecoder.hpp"

#include <AvTranscoder/codec/ICodec.hpp>
#include <AvTranscoder/stream/InputStream.hpp>
#include <AvTranscoder/data/decoded/VideoFrame.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>

namespace avtranscoder
{

VideoDecoder::VideoDecoder(InputStream& inputStream)
    : _inputStream(&inputStream)
    , _isSetup(false)
{
}

VideoDecoder::~VideoDecoder()
{
}

void VideoDecoder::setupDecoder(const ProfileLoader::Profile& profile)
{
    // check the given profile
    const bool isValid = ProfileLoader::checkVideoProfile(profile);
    if(!isValid && !profile.empty())
    {
        const std::string msg("Invalid video profile to setup decoder.");
        LOG_ERROR(msg)
        throw std::runtime_error(msg);
    }

    if(!profile.empty())
    {
        LOG_INFO("Setup video decoder with:\n" << profile)
    }

    VideoCodec& codec = _inputStream->getVideoCodec();

    // set threads before any other options
    if(profile.count(constants::avProfileThreads))
        codec.getOption(constants::avProfileThreads).setString(profile.at(constants::avProfileThreads));
    else
        codec.getOption(constants::avProfileThreads).setInt(codec.getAVCodecContext().thread_count);

    // set decoder options
    for(ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it)
    {
        if((*it).first == constants::avProfileIdentificator || (*it).first == constants::avProfileIdentificatorHuman ||
           (*it).first == constants::avProfileType || (*it).first == constants::avProfileThreads)
            continue;

        try
        {
            Option& decodeOption = codec.getOption((*it).first);
            decodeOption.setString((*it).second);
        }
        catch(std::exception& e)
        {
            LOG_WARN("VideoDecoder - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what())
        }
    }

    // open decoder
    _inputStream->getVideoCodec().openCodec();
    _isSetup = true;
}

bool VideoDecoder::decodeNextFrame(Frame& frameBuffer)
{
    bool decodeNextFrame = false;

    if(!_isSetup)
        setupDecoder();

    int got_frame = 0;
    while(!got_frame)
    {
        CodedData data;

        // reading
        const bool nextPacketRead = _inputStream->readNextPacket(data);

        // decoding
        // @note could be called several times to return the remaining frames (last call with an empty packet)
        // @see CODEC_CAP_DELAY
        const int ret = avcodec_decode_video2(&_inputStream->getVideoCodec().getAVCodecContext(), &frameBuffer.getAVFrame(),
                                              &got_frame, &data.getAVPacket());
        if(ret < 0)
        {
            throw std::runtime_error("An error occurred during video decoding: " + getDescriptionFromErrorCode(ret));
        }

        // if no frame could be decompressed
        if(!nextPacketRead && ret == 0 && got_frame == 0)
            decodeNextFrame = false;
        else
            decodeNextFrame = true;

        // if no frame read and decompressed
        if(!nextPacketRead && !decodeNextFrame)
        {
            data.clear();
            return false;
        }
    }
    return decodeNextFrame;
}

bool VideoDecoder::decodeNextFrame(Frame& frameBuffer, const std::vector<size_t> channelIndexArray)
{
    return false;
}

void VideoDecoder::flushDecoder()
{
    avcodec_flush_buffers(&_inputStream->getVideoCodec().getAVCodecContext());
}
}
