#include "AudioDecoder.hpp"

#include <AvTranscoder/codec/ICodec.hpp>
#include <AvTranscoder/stream/InputStream.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>

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
    , _frame(NULL)
    , _isSetup(false)
{
#if LIBAVCODEC_VERSION_MAJOR > 54
    _frame = av_frame_alloc();
#else
    _frame = avcodec_alloc_frame();
#endif
    if(_frame == NULL)
    {
        throw std::runtime_error("unable to setup frame buffer");
    }
}

AudioDecoder::~AudioDecoder()
{
    if(_frame != NULL)
    {
#if LIBAVCODEC_VERSION_MAJOR > 54
        av_frame_free(&_frame);
#else
#if LIBAVCODEC_VERSION_MAJOR > 53
        avcodec_free_frame(&_frame);
#else
        av_free(_frame);
#endif
#endif
        _frame = NULL;
    }
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
    if(!decodeNextFrame())
        return false;

    AVCodecContext& avCodecContext = _inputStream->getAudioCodec().getAVCodecContext();

    const int noAlignment = 1;
    const size_t decodedSize = av_samples_get_buffer_size(NULL, avCodecContext.channels, _frame->nb_samples,
                                                          avCodecContext.sample_fmt, noAlignment);
    if(decodedSize == 0)
        return false;

    AudioFrame& audioBuffer = static_cast<AudioFrame&>(frameBuffer);
    audioBuffer.setNbSamplesPerChannel(_frame->nb_samples);
    audioBuffer.resize(decodedSize);

    // @todo manage cases with data of frame not only on data[0] (use _frame.linesize)
    unsigned char* const src = _frame->data[0];
    unsigned char* dst = audioBuffer.getData();

    av_samples_copy(&dst, &src, 0, 0, _frame->nb_samples, avCodecContext.channels, avCodecContext.sample_fmt);

    return true;
}

bool AudioDecoder::decodeNextFrame(Frame& frameBuffer, const size_t channelIndex)
{
    if(!decodeNextFrame())
        return false;

    AVCodecContext& avCodecContext = _inputStream->getAudioCodec().getAVCodecContext();
    const size_t srcNbChannels = avCodecContext.channels;
    const size_t bytePerSample = av_get_bytes_per_sample((AVSampleFormat)_frame->format);

    const int dstNbChannels = 1;
    const int noAlignment = 1;
    const size_t decodedSize =
        av_samples_get_buffer_size(NULL, dstNbChannels, _frame->nb_samples, avCodecContext.sample_fmt, noAlignment);
    if(decodedSize == 0)
        return false;

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

    AudioFrame& audioBuffer = static_cast<AudioFrame&>(frameBuffer);
    audioBuffer.setNbSamplesPerChannel(_frame->nb_samples);
    audioBuffer.resize(decodedSize);

    // @todo manage cases with data of frame not only on data[0] (use _frame.linesize)
    unsigned char* src = _frame->data[0];
    unsigned char* dst = audioBuffer.getData();

    // offset
    src += channelIndex * bytePerSample;

    for(int sample = 0; sample < _frame->nb_samples; ++sample)
    {
        memcpy(dst, src, bytePerSample);
        dst += bytePerSample;
        src += bytePerSample * srcNbChannels;
    }

    return true;
}

bool AudioDecoder::decodeNextFrame()
{
    if(!_isSetup)
        setupDecoder();

    int got_frame = 0;
    while(!got_frame)
    {
        CodedData data;

        bool nextPacketRead = _inputStream->readNextPacket(data);
        if(!nextPacketRead) // error or end of file
            data.clear();

        int ret = avcodec_decode_audio4(&_inputStream->getAudioCodec().getAVCodecContext(), _frame, &got_frame,
                                        &data.getAVPacket());
        if(!nextPacketRead && ret == 0 && got_frame == 0) // no frame could be decompressed
            return false;

        if(ret < 0)
        {
            throw std::runtime_error("an error occured during audio decoding" + getDescriptionFromErrorCode(ret));
        }
    }
    return true;
}

void AudioDecoder::flushDecoder()
{
    avcodec_flush_buffers(&_inputStream->getAudioCodec().getAVCodecContext());
}
}
