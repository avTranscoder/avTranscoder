#ifndef _AV_TRANSCODER_DECODER_VIDEO_DECODER_HPP_
#define _AV_TRANSCODER_DECODER_VIDEO_DECODER_HPP_

#include "IDecoder.hpp"

namespace avtranscoder
{

class InputStream;

class AvExport VideoDecoder : public IDecoder
{
public:
    VideoDecoder(InputStream& inputStream);
    ~VideoDecoder();

    void setupDecoder(const ProfileLoader::Profile& profile = ProfileLoader::Profile());

    bool decodeNextFrame(Frame& frameBuffer);
    bool decodeNextFrame(Frame& frameBuffer, const std::vector<size_t> channelIndexArray);

    void flushDecoder();

private:
    InputStream* _inputStream; ///< Stream from which we read next frames (no ownership, has link)

    bool _isSetup;
};
}

#endif
