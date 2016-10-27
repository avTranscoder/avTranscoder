#include "AudioFrame.hpp"

#include <AvTranscoder/util.hpp>

extern "C" {
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
}

#include <stdexcept>

namespace avtranscoder
{

AudioFrameDesc::AudioFrameDesc(const size_t sampleRate, const size_t nbChannels, const AVSampleFormat sampleFormat)
    : _sampleRate(sampleRate)
    , _nbChannels(nbChannels)
    , _sampleFormat(sampleFormat)
{
}

AudioFrameDesc::AudioFrameDesc(const size_t sampleRate, const size_t nbChannels, const std::string& sampleFormatName)
    : _sampleRate(sampleRate)
    , _nbChannels(nbChannels)
    , _sampleFormat(getAVSampleFormat(sampleFormatName))
{
}

void AudioFrameDesc::setParameters(const ProfileLoader::Profile& profile)
{
    // sample rate
    if(profile.count(constants::avProfileSampleRate))
        _sampleRate = atoi(profile.find(constants::avProfileSampleRate)->second.c_str());
    // channel
    if(profile.count(constants::avProfileChannel))
        _nbChannels = atoi(profile.find(constants::avProfileChannel)->second.c_str());
    // sample format
    if(profile.count(constants::avProfileSampleFormat))
        _sampleFormat = getAVSampleFormat(profile.find(constants::avProfileSampleFormat)->second.c_str());
}

AudioFrame::AudioFrame(const AudioFrameDesc& desc, const bool forceDataAllocation)
    : Frame()
    , _desc(desc)
{
    // Set Frame properties
    av_frame_set_sample_rate(_frame, desc._sampleRate);
    av_frame_set_channels(_frame, desc._nbChannels);
    av_frame_set_channel_layout(_frame, av_get_default_channel_layout(desc._nbChannels));
    _frame->format = desc._sampleFormat;
    _frame->nb_samples = desc._sampleRate / 25.; // cannot be known before calling avcodec_decode_audio4

    if(forceDataAllocation)
        allocateData();
}

std::string AudioFrame::getChannelLayoutDesc() const
{
    char buf[512];
    av_get_channel_layout_string(buf, sizeof(buf), getNbChannels(), getChannelLayout());
    return std::string(buf);
}

AudioFrame::~AudioFrame()
{
    if(_dataAllocated)
        freeData();
}

size_t AudioFrame::getSize() const
{
    if(getSampleFormat() == AV_SAMPLE_FMT_NONE)
    {
        LOG_WARN("Incorrect sample format when get size of audio frame: return a size of 0.")
        return 0;
    }

    const size_t size = getNbSamplesPerChannel() * getNbChannels() * av_get_bytes_per_sample(getSampleFormat());
    if(size == 0)
    {
        std::stringstream msg;
        msg << "Unable to determine audio buffer size:" << std::endl;
        msg << "nb sample per channel = " << getNbSamplesPerChannel() << std::endl;
        msg << "channels = " << getNbChannels() << std::endl;
        msg << "bytes per sample = " << av_get_bytes_per_sample(getSampleFormat()) << std::endl;
        throw std::runtime_error(msg.str());
    }
    return size;
}

void AudioFrame::allocateData()
{
    // Set Frame properties
    av_frame_set_sample_rate(_frame, _desc._sampleRate);
    av_frame_set_channels(_frame, _desc._nbChannels);
    av_frame_set_channel_layout(_frame, av_get_default_channel_layout(_desc._nbChannels));
    _frame->format = _desc._sampleFormat;
    _frame->nb_samples = _desc._sampleRate / 25.; // cannot be known before calling avcodec_decode_audio4

    // Allocate data
    const int align = 0;
    const int ret =
        av_samples_alloc(_frame->data, _frame->linesize, _frame->channels, _frame->nb_samples, _desc._sampleFormat, align);
    if(ret < 0)
    {
        std::stringstream os;
        os << "Unable to allocate an audio frame of ";
        os << "sample rate = " << _frame->sample_rate << ", ";
        os << "nb channels = " << _frame->channels << ", ";
        os << "channel layout = " << av_get_channel_name(_frame->channels) << ", ";
        os << "nb samples = " << _frame->nb_samples << ", ";
        os << "sample format = " << getSampleFormatName(_desc._sampleFormat);
        throw std::runtime_error(os.str());
    }
    _dataAllocated = true;
}

void AudioFrame::freeData()
{
    av_freep(&_frame->data[0]);
    _dataAllocated = false;
}

void AudioFrame::assign(const unsigned char value)
{
    // Create the audio buffer
    // The buffer will be freed in destructor of based class
    const int audioSize = getSize();
    unsigned char* audioBuffer = new unsigned char[audioSize];
    memset(audioBuffer, value, audioSize);

    // Fill the picture
    assign(audioBuffer);
}

void AudioFrame::assign(const unsigned char* ptrValue)
{
    const int align = 0;
    const int ret = av_samples_fill_arrays(_frame->data, _frame->linesize, ptrValue, getNbChannels(),
                                           getNbSamplesPerChannel(), getSampleFormat(), align);
    if(ret < 0)
    {
        std::stringstream os;
        os << "Unable to assign an audio buffer: " << getDescriptionFromErrorCode(ret);
        throw std::runtime_error(os.str());
    }
}
}
