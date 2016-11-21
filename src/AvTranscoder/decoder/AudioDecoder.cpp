#include "AudioDecoder.hpp"

#include <AvTranscoder/util.hpp>
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
            LOG_WARN("AudioDecoder - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what())
        }
    }

    // open decoder
    _inputStream->getAudioCodec().openCodec();
    _isSetup = true;
}

bool AudioDecoder::decodeNextFrame(IFrame& frameBuffer)
{
    bool decodeNextFrame = false;
    const size_t channelLayout = frameBuffer.getAVFrame().channel_layout;

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
        int ret = avcodec_decode_audio4(&_inputStream->getAudioCodec().getAVCodecContext(), &frameBuffer.getAVFrame(),
                                        &got_frame, &data.getAVPacket());
        if(ret < 0)
        {
            throw std::runtime_error("An error occurred during audio decoding: " + getDescriptionFromErrorCode(ret));
        }

        // fixed channel layout value after decoding
        frameBuffer.getAVFrame().channel_layout = channelLayout;

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

bool AudioDecoder::decodeNextFrame(IFrame& frameBuffer, const std::vector<size_t> channelIndexArray)
{
    AVCodecContext& avCodecContext = _inputStream->getAudioCodec().getAVCodecContext();
    const size_t srcNbChannels = avCodecContext.channels;

    // check if each expected channel exists
    for(std::vector<size_t>::const_iterator channelIndex = channelIndexArray.begin();
        channelIndex != channelIndexArray.end(); ++channelIndex)
    {
        if((*channelIndex) > srcNbChannels - 1)
        {
            std::stringstream msg;
            msg << "The channel at index ";
            msg << (*channelIndex);
            msg << " doesn't exist (srcNbChannels = ";
            msg << srcNbChannels;
            msg << ").";
            throw std::runtime_error(msg.str());
        }
    }

    // if all channels of the stream are extracted
    if(srcNbChannels == channelIndexArray.size())
        return decodeNextFrame(frameBuffer);

    // else decode all data in an intermediate buffer
    AudioFrame& audioBuffer = static_cast<AudioFrame&>(frameBuffer);
    AudioFrame allDataOfNextFrame(AudioFrameDesc(audioBuffer.getSampleRate(), srcNbChannels, getSampleFormatName(audioBuffer.getSampleFormat())), false);
    if(!decodeNextFrame(allDataOfNextFrame))
        return false;

    const size_t bytePerSample = audioBuffer.getBytesPerSample();
    const int dstNbChannels = channelIndexArray.size();
    const int noAlignment = 0;
    const size_t decodedSize = av_samples_get_buffer_size(NULL, dstNbChannels, frameBuffer.getAVFrame().nb_samples,
                                                          avCodecContext.sample_fmt, noAlignment);
    if(decodedSize == 0)
        return false;

    // update the output frame
    audioBuffer.copyProperties(allDataOfNextFrame);
    audioBuffer.setNbSamplesPerChannel(allDataOfNextFrame.getNbSamplesPerChannel());
    if(! audioBuffer.isDataAllocated())
        audioBuffer.allocateData();

    // @todo manage cases with data of frame not only on data[0] (use _frame.linesize)
    unsigned char* src = allDataOfNextFrame.getData()[0];
    unsigned char* dst = audioBuffer.getData()[0];

    // extract one or more channels
    for(size_t sample = 0; sample < allDataOfNextFrame.getNbSamplesPerChannel(); ++sample)
    {
        // offset in source buffer
        src += channelIndexArray.at(0) * bytePerSample;

        for(size_t i = 0; i < channelIndexArray.size(); ++i)
        {
            memcpy(dst, src, bytePerSample);
            dst += bytePerSample;

            // shift to the corresponding sample in the next channel of the current layout
            if(i < channelIndexArray.size() - 1)
                src += (channelIndexArray.at(i + 1) - channelIndexArray.at(i)) * bytePerSample;
            // else shift to the next layout
            else
            {
                src += (srcNbChannels - channelIndexArray.at(i)) * bytePerSample;
                break;
            }
        }
    }

    return true;
}

void AudioDecoder::flushDecoder()
{
    avcodec_flush_buffers(&_inputStream->getAudioCodec().getAVCodecContext());
}
}
