#ifndef _AV_TRANSCODER_ESSENCE_STREAM_IDECODER_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_IDECODER_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/data/decoded/IFrame.hpp>
#include <AvTranscoder/profile/ProfileLoader.hpp>

namespace avtranscoder
{

class AvExport IDecoder
{
protected:
    IDecoder()
        : _decoded_frames_counter(0)
    {
    }

public:



    virtual ~IDecoder(){};

    /**
     * @brief Setup the decoder
     * @param profile: set decoder parameters from the given profile
     * @note Open the decoder.
     */
    virtual void setupDecoder(const ProfileLoader::Profile& profile = ProfileLoader::Profile()) {}

    /**
     * @brief Decode next frame
     * @param frameBuffer: the frame decoded
     * @warn the frameBuffer reference belongs to the decoder and is valid only until the
     * next call to this function or until closing or flushing the
     * decoder. The caller may not write to it.
     * @return status of decoding
     */
    virtual bool decodeNextFrame(IFrame& frameBuffer) = 0;

    /**
     * @brief Decode substream of next frame
     * @param frameBuffer: the frame decoded
     * @param channelIndexArray: list of channels to extract
     * @return status of decoding
     */
    virtual bool decodeNextFrame(IFrame& frameBuffer, const std::vector<size_t> channelIndexArray) = 0;

    /**
     * @brief Set the next frame of the input stream (which bypass the work of decoding)
     * @note Not yet implemented for VideoDecoder and AudioDecoder
     * @param inputFrame: the new next frame
     */
    virtual void setNextFrame(IFrame& inputFrame) {}

    /**
     * @brief Reset the internal decoder state / flush internal buffers.
     * @note Should be called when seeking or when switching to a different stream.
     * @note Not sense for generators.
     */
    virtual void flushDecoder() {}

    size_t getNbDecodedFrames() { return _decoded_frames_counter; }

protected:
    void incrementNbDecodedFrames(const size_t& nb_frames = 1) {
        _decoded_frames_counter += nb_frames;
    }

private:
    size_t _decoded_frames_counter;
};
}

#endif
