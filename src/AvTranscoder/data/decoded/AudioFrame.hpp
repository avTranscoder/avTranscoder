#ifndef _AV_TRANSCODER_FRAME_AUDIO_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_AUDIO_FRAME_HPP_

#include "Frame.hpp"
#include <AvTranscoder/profile/ProfileLoader.hpp>

namespace avtranscoder
{

/**
 * @brief Description to create an audio frame.
 * This corresponds to the number of samples, which corresponds to one video frame.
 */
struct AvExport AudioFrameDesc
{
public:
    AudioFrameDesc(const size_t sampleRate = 0, const size_t channels = 0,
                   const AVSampleFormat sampleFormat = AV_SAMPLE_FMT_NONE);
    AudioFrameDesc(const size_t sampleRate, const size_t channels, const std::string& sampleFormatName);

    /**
     * @brief Set the attributes from the given profile.
     * @see Profile
     */
    void setParameters(const ProfileLoader::Profile& profile);

public:
    size_t _sampleRate;
    size_t _nbChannels;
    AVSampleFormat _sampleFormat;
};

/**
 * @brief This class describes decoded audio data.
 */
class AvExport AudioFrame : public Frame
{
public:
    /**
     * @note Allocated data will be initialized to silence.
     */
    AudioFrame(const AudioFrameDesc& ref);
    AudioFrame(const Frame& otherFrame);

    size_t getSampleRate() const;
    size_t getNbChannels() const;
    size_t getChannelLayout() const;
    AVSampleFormat getSampleFormat() const { return static_cast<AVSampleFormat>(_frame->format); }
    size_t getNbSamplesPerChannel() const { return _frame->nb_samples; }
    AudioFrameDesc desc() const { return AudioFrameDesc(getSampleRate(), getNbChannels(), getSampleFormat()); }

    size_t getSize() const; ///< in bytes

    void setNbChannels(const size_t nbChannels);
    void setChannelLayout(const size_t channelLayout);
    void setNbSamplesPerChannel(const size_t nbSamples) { _frame->nb_samples = nbSamples; }

    /**
     * @brief Assign the given value to all the data of the audio frame.
     */
    void assign(const unsigned char value);

    /**
     * @brief Assign the given ptr of data to the data of the audio frame.
     * @warning the given ptr should have the size of the audio frame..
     * @see getSize
     */
    void assign(const unsigned char* ptrValue);

private:
    /**
     * @brief Allocate the audio buffer of the frame.
     */
    void allocateAVSample(const AudioFrameDesc& ref);

    /**
     * @note To allocate new audio buffer if needed.
     * @see allocateAVSample
     */
    friend class AudioGenerator;
};
}

#endif
