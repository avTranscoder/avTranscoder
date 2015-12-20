#ifndef _AV_TRANSCODER_FRAME_AUDIO_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_AUDIO_FRAME_HPP_

#include "Frame.hpp"
#include <AvTranscoder/profile/ProfileLoader.hpp>

extern "C" {
#include <libavutil/samplefmt.h>
}

namespace avtranscoder
{

/// @brief Description of a number of samples, which corresponds to one video frame
class AvExport AudioFrameDesc
{
public:
    AudioFrameDesc(const size_t sampleRate = 0, const size_t channels = 0,
                   const AVSampleFormat sampleFormat = AV_SAMPLE_FMT_NONE);

    AudioFrameDesc(const size_t sampleRate, const size_t channels, const std::string& sampleFormat);

    size_t getSampleRate() const { return _sampleRate; }
    size_t getChannels() const { return _channels; }
    AVSampleFormat getSampleFormat() const { return _sampleFormat; }
    std::string getSampleFormatName() const;
    double getFps() const { return _fps; }

    size_t getDataSize() const;

    void setSampleRate(const size_t sampleRate) { _sampleRate = sampleRate; }
    void setChannels(const size_t channels) { _channels = channels; }
    void setSampleFormat(const std::string& sampleFormatName);
    void setSampleFormat(const AVSampleFormat sampleFormat) { _sampleFormat = sampleFormat; }
    void setFps(const double fps) { _fps = fps; }

    void setParameters(const ProfileLoader::Profile& profile);

private:
    size_t _sampleRate;
    size_t _channels;
    AVSampleFormat _sampleFormat;
    double _fps;
};

class AvExport AudioFrame : public Frame
{
public:
    AudioFrame(const AudioFrameDesc& ref)
        : Frame(ref.getDataSize())
        , _audioFrameDesc(ref)
        , _nbSamples(0)
    {
    }

    const AudioFrameDesc& desc() const { return _audioFrameDesc; }

    size_t getNbSamples() const { return _nbSamples; }
    void setNbSamples(size_t nbSamples) { _nbSamples = nbSamples; }

private:
    const AudioFrameDesc _audioFrameDesc;
    size_t _nbSamples;
};
}

#endif
