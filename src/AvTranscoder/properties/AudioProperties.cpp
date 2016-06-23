#include "AudioProperties.hpp"

#include <AvTranscoder/properties/util.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
}

#include <stdexcept>

namespace avtranscoder
{

AudioProperties::AudioProperties(const FileProperties& fileProperties, const size_t index)
    : StreamProperties(fileProperties, index)
{
}

std::string AudioProperties::getSampleFormatName() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");

    const char* fmtName = av_get_sample_fmt_name(_codecContext->sample_fmt);
    if(!fmtName)
        throw std::runtime_error("unknown sample format");

    return std::string(fmtName);
}

std::string AudioProperties::getSampleFormatLongName() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");

    switch(_codecContext->sample_fmt)
    {
        case AV_SAMPLE_FMT_NONE:
            return "none";
        case AV_SAMPLE_FMT_U8:
            return "unsigned 8 bits";
        case AV_SAMPLE_FMT_S16:
            return "signed 16 bits";
        case AV_SAMPLE_FMT_S32:
            return "signed 32 bits";
        case AV_SAMPLE_FMT_FLT:
            return "float";
        case AV_SAMPLE_FMT_DBL:
            return "double";
        case AV_SAMPLE_FMT_U8P:
            return "unsigned 8 bits, planar";
        case AV_SAMPLE_FMT_S16P:
            return "signed 16 bits, planar";
        case AV_SAMPLE_FMT_S32P:
            return "signed 32 bits, planar";
        case AV_SAMPLE_FMT_FLTP:
            return "float, planar";
        case AV_SAMPLE_FMT_DBLP:
            return "double, planar";
        case AV_SAMPLE_FMT_NB:
            return "number of sample formats";
    }
    return "unknown sample format";
}

std::string AudioProperties::getChannelLayout() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");

    char buf1[1024];
    av_get_channel_layout_string(buf1, sizeof(buf1), -1, _codecContext->channel_layout);
    return std::string(buf1);
}

std::string AudioProperties::getChannelName() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");

    const char* channelName = av_get_channel_name(_codecContext->channel_layout);
    if(!channelName)
        throw std::runtime_error("unknown channel name");

    return std::string(channelName);
}

std::string AudioProperties::getChannelDescription() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");

#ifdef AVTRANSCODER_FFMPEG_DEPENDENCY
    const char* channelDescription = av_get_channel_description(_codecContext->channel_layout);
    if(!channelDescription)
        throw std::runtime_error("unknown channel description");
    return std::string(channelDescription);
#else
    throw std::runtime_error("can't access channel description");
#endif
}

size_t AudioProperties::getBitRate() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");

    // return bit rate of stream if present
    if(_codecContext->bit_rate)
        return _codecContext->bit_rate;

    LOG_WARN("The bitrate of the stream '" << _streamIndex << "' of file '" << _formatContext->filename << "' is unknown.")
    LOG_INFO("Compute the audio bitrate (suppose PCM audio data).")

    const int bitsPerSample = av_get_bits_per_sample(_codecContext->codec_id); // 0 if unknown for the given codec
    return getSampleRate() * getNbChannels() * bitsPerSample;
}

size_t AudioProperties::getSampleRate() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");
    return _codecContext->sample_rate;
}

size_t AudioProperties::getNbChannels() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");
    return _codecContext->channels;
}

size_t AudioProperties::getNbSamples() const
{
    if(!_formatContext)
        throw std::runtime_error("unknown format context");
    size_t nbSamples = _formatContext->streams[_streamIndex]->nb_frames;
    if(nbSamples == 0)
        nbSamples = getSampleRate() * getNbChannels() * getDuration();
    return nbSamples;
}

size_t AudioProperties::getTicksPerFrame() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");
    return _codecContext->ticks_per_frame;
}

PropertyVector& AudioProperties::fillVector(PropertyVector& data) const
{
    // Add properties of base class
    PropertyVector basedProperty;
    StreamProperties::fillVector(basedProperty);
    data.insert(data.begin(), basedProperty.begin(), basedProperty.end());

    addProperty(data, "sampleFormatName", &AudioProperties::getSampleFormatName);
    addProperty(data, "sampleFormatLongName", &AudioProperties::getSampleFormatLongName);
    addProperty(data, "bitRate", &AudioProperties::getBitRate);
    addProperty(data, "sampleRate", &AudioProperties::getSampleRate);
    addProperty(data, "nbSamples", &AudioProperties::getNbSamples);
    addProperty(data, "nbChannels", &AudioProperties::getNbChannels);
    addProperty(data, "channelLayout", &AudioProperties::getChannelLayout);
    addProperty(data, "channelName", &AudioProperties::getChannelName);
    addProperty(data, "channelDescription", &AudioProperties::getChannelDescription);
    addProperty(data, "ticksPerFrame", &AudioProperties::getTicksPerFrame);

    return data;
}

std::ostream& operator<<(std::ostream& flux, const AudioProperties& audioProperties)
{
    flux << std::left;
    flux << detail::separator << " Audio stream " << detail::separator << std::endl;

    PropertyVector properties = audioProperties.asVector();
    for(PropertyVector::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        flux << std::setw(detail::keyWidth) << it->first << ": " << it->second << std::endl;
    }

    return flux;
}
}
