#ifndef _AV_TRANSCODER_AUDIOREADER_HPP
#define _AV_TRANSCODER_AUDIOREADER_HPP

#include "IReader.hpp"

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/properties/AudioProperties.hpp>

namespace avtranscoder
{

class AvExport AudioReader : public IReader
{
public:
    //@{
    // @param sampleRate: if 0, get sample rate of source
    // @param nbChannels: if 0, get number of channels of source
    // @param sampleFormat: pcm_16le by default (to listen)
    //
    AudioReader(const std::string& filename, const size_t audioStreamIndex, const size_t sampleRate = 0,
                const size_t nbChannels = 0, const std::string& sampleFormat = "s16");
    AudioReader(InputFile& inputFile, const size_t audioStreamIndex, const size_t sampleRate = 0,
                const size_t nbChannels = 0, const std::string& sampleFormat = "s16");

    ~AudioReader();

    //@{
    // @brief Output info
    size_t getSampleRate();
    size_t getChannels();
    AVSampleFormat getSampleFormat();
    //@}

    // @brief Input info
    const AudioProperties* getAudioProperties() const { return _audioStreamProperties; }

    void printInfo();

private:
    void init();

private:
    const AudioProperties* _audioStreamProperties; ///< Properties of the source audio stream read (no ownership, has link)

    //@{
    // @brief Output info
    size_t _sampleRate;
    size_t _nbChannels;
    AVSampleFormat _sampleFormat;
    //@}
};
}

#endif
