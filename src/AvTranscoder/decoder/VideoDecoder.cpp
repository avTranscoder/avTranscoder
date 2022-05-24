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
    if(_isSetup)
        _inputStream->getVideoCodec().closeCodec();
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
           (*it).first == constants::avProfileType || (*it).first == constants::avProfileCodec ||
           (*it).first == constants::avProfileThreads)
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
    codec.openCodec();
    _isSetup = true;
}

bool VideoDecoder::decodeNextFrame(IFrame& frameBuffer)
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
        int ret = avcodec_send_packet(&_inputStream->getVideoCodec().getAVCodecContext(), &data.getAVPacket());

        if (ret < 0 && (nextPacketRead || ret != AVERROR_EOF))
            throw std::runtime_error("An error occurred sending video packet to decoder: " + getDescriptionFromErrorCode(ret));

        ret = avcodec_receive_frame(&_inputStream->getVideoCodec().getAVCodecContext(), &frameBuffer.getAVFrame());

        if (ret == 0)
            got_frame = true;
        else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            got_frame = false;
        else
            throw std::runtime_error("An error occurred receiving video packet from decoder: " + getDescriptionFromErrorCode(ret));

        // if no frame could be decompressed
        if ((!nextPacketRead && ret == 0) || !got_frame)
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

    if(decodeNextFrame)
        incrementNbDecodedFrames();

    return decodeNextFrame;
}

bool VideoDecoder::decodeNextFrame(IFrame& frameBuffer, const std::vector<size_t> channelIndexArray)
{
    LOG_WARN("Decoding of a specific video channel is not supported.");
    return false;
}

void VideoDecoder::flushDecoder()
{
    avcodec_flush_buffers(&_inputStream->getVideoCodec().getAVCodecContext());
}
}
