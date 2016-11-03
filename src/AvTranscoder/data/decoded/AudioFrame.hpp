#ifndef _AV_TRANSCODER_FRAME_AUDIO_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_AUDIO_FRAME_HPP_

#include "IFrame.hpp"
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
    AudioFrameDesc(const size_t sampleRate, const size_t channels, const std::string& sampleFormatName);
    AudioFrameDesc(const ProfileLoader::Profile& profile);

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
class AvExport AudioFrame : public IFrame
{
private:
    AudioFrame(const AudioFrame& otherFrame);
    AudioFrame& operator=(const AudioFrame& otherFrame);

public:
    AudioFrame(const AudioFrameDesc& desc, const bool forceDataAllocation = true);
    ~AudioFrame();

    /**
     * @brief Allocated data will be initialized to silence.
     * @warning The allocated data should be freed by the caller.
     * @see freeData
     */
    void allocateData();
    void freeData();
    size_t getSize() const;

    size_t getSampleRate() const { return av_frame_get_sample_rate(_frame); }
    size_t getNbChannels() const { return av_frame_get_channels(_frame); }
    size_t getChannelLayout() const { return av_frame_get_channel_layout(_frame); }
    std::string getChannelLayoutDesc() const; ///< Get a description of a channel layout (example: '5.1').
    AVSampleFormat getSampleFormat() const { return static_cast<AVSampleFormat>(_frame->format); }
    size_t getBytesPerSample() const;  ///< 0 if unknown sample format
    size_t getNbSamplesPerChannel() const { return _frame->nb_samples; }

    /**
     * @brief This methods dynamically updates the size that the data buffer would occupy if allocated.
     * @warning If the data buffer is already allocated, this could lead to memory leaks or seg fault.
     */
    void setNbSamplesPerChannel(const size_t nbSamples) { _frame->nb_samples = nbSamples; }

    void assignBuffer(const unsigned char* ptrValue);

private:
    /**
     * @brief The number of samples of a frame cannot be known before calling avcodec_decode_audio4,
     * and can be variable in a same stream. Because we need to allocate some frames without knowing this parameter,
     * we access here a default number of samples.
     * @note This value depends on the sample rate (example: 1920 samples at 48kHz).
     * @return the number of samples of our default AudioFrame.
     * @see setNbSamplesPerChannel
     */
    size_t getDefaultNbSamples() const;

private:
    /**
     * @brief Description of the frame to allocate.
     * @warning This description could be different from the current frame (a decoder could have reseted it).
     * We need to keep this description to allocate again the frame even if it was reseted.
     */
    const AudioFrameDesc _desc;
};
}

#endif
