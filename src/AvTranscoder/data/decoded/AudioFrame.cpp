#include "AudioFrame.hpp"

#include <AvTranscoder/util.hpp>

extern "C" {
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
}

#include <stdexcept>

namespace avtranscoder
{

AudioFrameDesc::AudioFrameDesc(const size_t sampleRate, const size_t nbChannels, const std::string& sampleFormatName)
    : _sampleRate(sampleRate)
    , _nbChannels(nbChannels)
    , _sampleFormat(getAVSampleFormat(sampleFormatName))
{
}

AudioFrameDesc::AudioFrameDesc(const ProfileLoader::Profile& profile)
    : _sampleRate(0)
    , _nbChannels(0)
    , _sampleFormat(AV_SAMPLE_FMT_NONE)
{
    setParameters(profile);
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
    : IFrame()
    , _desc(desc)
{
    // Set Frame properties
    av_frame_set_sample_rate(_frame, desc._sampleRate);
    av_frame_set_channels(_frame, desc._nbChannels);
    av_frame_set_channel_layout(_frame, av_get_default_channel_layout(desc._nbChannels));
    _frame->format = desc._sampleFormat;
    _frame->nb_samples = getDefaultNbSamples();

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
    if(_frame->buf[0])
        av_frame_unref(_frame);
    if(_dataAllocated)
        freeData();
}

size_t AudioFrame::getBytesPerSample() const
{
    return av_get_bytes_per_sample(getSampleFormat());
}

size_t AudioFrame::getSize() const
{
    if(getSampleFormat() == AV_SAMPLE_FMT_NONE)
    {
        LOG_WARN("Incorrect sample format when get size of audio frame: return a size of 0.")
        return 0;
    }

    const size_t size = getNbSamplesPerChannel() * getNbChannels() * getBytesPerSample();
    if(size == 0)
    {
        std::stringstream msg;
        msg << "Unable to determine audio buffer size:" << std::endl;
        msg << "nb sample per channel = " << getNbSamplesPerChannel() << std::endl;
        msg << "channels = " << getNbChannels() << std::endl;
        msg << "bytes per sample = " << getBytesPerSample() << std::endl;
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
    if(_frame->nb_samples == 0)
        _frame->nb_samples = getDefaultNbSamples();

    // Allocate data
    const int align = 0;
    const int ret =
        av_samples_alloc(_frame->data, _frame->linesize, _frame->channels, _frame->nb_samples, _desc._sampleFormat, align);
    if(ret < 0)
    {
        const std::string formatName = getSampleFormatName(_desc._sampleFormat);
        std::stringstream msg;
        msg << "Unable to allocate an audio frame of ";
        msg << "sample rate = " << _frame->sample_rate << ", ";
        msg << "nb channels = " << _frame->channels << ", ";
        msg << "channel layout = " << av_get_channel_name(_frame->channels) << ", ";
        msg << "nb samples = " << _frame->nb_samples << ", ";
        msg << "sample format = " << (formatName.empty() ? "none" : formatName);
        LOG_ERROR(msg.str())
        throw std::bad_alloc();
    }
    _dataAllocated = true;
}

void AudioFrame::freeData()
{
    av_freep(&_frame->data[0]);
    _dataAllocated = false;
}

void AudioFrame::assignBuffer(const unsigned char* ptrValue)
{
    const int align = 0;
    const int ret = av_samples_fill_arrays(_frame->data, _frame->linesize, ptrValue, getNbChannels(),
                                           getNbSamplesPerChannel(), getSampleFormat(), align);
    if(ret < 0)
    {
        std::stringstream msg;
        msg << "Unable to assign an audio buffer: " << getDescriptionFromErrorCode(ret);
        throw std::runtime_error(msg.str());
    }
}

size_t AudioFrame::getDefaultNbSamples() const
{
    return _desc._sampleRate / 25.;
}

}
