#include "AudioDecoder.hpp"

#include <AvTranscoder/codec/ICodec.hpp>
#include <AvTranscoder/stream/InputStream.hpp>
#include <AvTranscoder/data/decoded/AudioFrame.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
}

#include <stdexcept>
#include <sstream>

namespace avtranscoder
{

AudioDecoder::AudioDecoder(InputStream& inputStream)
    : _inputStream(&inputStream)
    , _isSetup(false)
{
}

AudioDecoder::~AudioDecoder()
{
}

void AudioDecoder::setupDecoder(const ProfileLoader::Profile& profile)
{
    // check the given profile
    const bool isValid = ProfileLoader::checkAudioProfile(profile);
    if(!isValid && !profile.empty())
    {
        const std::string msg("Invalid audio profile to setup decoder.");
        LOG_ERROR(msg)
        throw std::runtime_error(msg);
    }

    if(!profile.empty())
    {
        LOG_INFO("Setup audio decoder with:\n" << profile)
    }

    AudioCodec& codec = _inputStream->getAudioCodec();

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
            LOG_WARN("AudioDecoder - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what())
        }
    }

    // open decoder
    _inputStream->getAudioCodec().openCodec();
    _isSetup = true;
}

bool AudioDecoder::decodeNextFrame(Frame& frameBuffer)
{
    bool decodeNextFrame = false;

    if(!_isSetup)
        setupDecoder();

    int got_frame = 0;
    while(!got_frame)
    {
        CodedData data;

        const bool nextPacketRead = _inputStream->readNextPacket(data);
        // if error or end of file
        if(!nextPacketRead)
        {
            data.clear();
            return false;
        }

        // decoding
        int ret = avcodec_decode_audio4(&_inputStream->getAudioCodec().getAVCodecContext(), &frameBuffer.getAVFrame(),
                                        &got_frame, &data.getAVPacket());
        if(ret < 0)
        {
            throw std::runtime_error("an error occured during audio decoding" + getDescriptionFromErrorCode(ret));
        }

        // if no frame could be decompressed
        if(!nextPacketRead && ret == 0 && got_frame == 0)
            decodeNextFrame = false;
        else
            decodeNextFrame = true;
    }
    return decodeNextFrame;
}

bool AudioDecoder::decodeNextFrame(Frame& frameBuffer, const size_t channelIndex)
{
    AudioFrame& audioBuffer = static_cast<AudioFrame&>(frameBuffer);

    // if no need to extract one channel in the audio stream
    if(audioBuffer.getNbChannels() == 1 && channelIndex == 0)
    {
        return decodeNextFrame(frameBuffer);
    }

    // decode all data of the next frame
    AudioFrame allDataOfNextFrame(audioBuffer);
    if(!decodeNextFrame(allDataOfNextFrame))
        return false;

    AVCodecContext& avCodecContext = _inputStream->getAudioCodec().getAVCodecContext();
    const size_t srcNbChannels = avCodecContext.channels;
    const size_t bytePerSample = av_get_bytes_per_sample((AVSampleFormat)frameBuffer.getAVFrame().format);

    const int dstNbChannels = 1;
    const int noAlignment = 0;
    const size_t decodedSize = av_samples_get_buffer_size(NULL, dstNbChannels, frameBuffer.getAVFrame().nb_samples,
                                                          avCodecContext.sample_fmt, noAlignment);
    if(decodedSize == 0)
        return false;

    // check if the expected channel exists
    if(channelIndex > srcNbChannels - 1)
    {
        std::stringstream msg;
        msg << "The channel at index ";
        msg << channelIndex;
        msg << " doesn't exist (srcNbChannels = ";
        msg << srcNbChannels;
        msg << ").";
        throw std::runtime_error(msg.str());
    }

    // copy frame properties of decoded frame
    audioBuffer.copyProperties(allDataOfNextFrame);
    av_frame_set_channels(&audioBuffer.getAVFrame(), 1);
    av_frame_set_channel_layout(&audioBuffer.getAVFrame(), AV_CH_LAYOUT_MONO);
    audioBuffer.setNbSamplesPerChannel(allDataOfNextFrame.getNbSamplesPerChannel());

    // @todo manage cases with data of frame not only on data[0] (use _frame.linesize)
    unsigned char* src = allDataOfNextFrame.getData()[0];
    unsigned char* dst = audioBuffer.getData()[0];

    // offset
    src += channelIndex * bytePerSample;

    // extract one channel
    for(int sample = 0; sample < allDataOfNextFrame.getAVFrame().nb_samples; ++sample)
    {
        memcpy(dst, src, bytePerSample);
        dst += bytePerSample;
        src += bytePerSample * srcNbChannels;
    }

    return true;
}

void AudioDecoder::flushDecoder()
{
    avcodec_flush_buffers(&_inputStream->getAudioCodec().getAVCodecContext());
}
}
