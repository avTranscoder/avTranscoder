#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_AUDIO_HPP_

#include "IDecoder.hpp"

namespace avtranscoder
{

class InputStream;

class AvExport AudioDecoder : public IDecoder
{
public:
    AudioDecoder(InputStream& inputStream);
    ~AudioDecoder();

    void setupDecoder(const ProfileLoader::Profile& profile = ProfileLoader::Profile());

    bool decodeNextFrame(Frame& frameBuffer);
    bool decodeNextFrame(Frame& frameBuffer, const std::vector<size_t> channelsIndex);

    void flushDecoder();

private:
    InputStream* _inputStream; ///< Stream from which we read next frames (no ownership, has link)

    bool _isSetup;
};
}

#endif
