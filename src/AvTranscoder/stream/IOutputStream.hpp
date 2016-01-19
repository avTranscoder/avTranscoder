#ifndef _AV_TRANSCODER_STREAM_I_OUTPUT_STREAM_HPP_
#define _AV_TRANSCODER_STREAM_I_OUTPUT_STREAM_HPP_

#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/data/coded/CodedData.hpp>

namespace avtranscoder
{

class AvExport IOutputStream
{
public:
    /**
     * @brief define wrapping result status
     **/
    enum EWrappingStatus
    {
        eWrappingSuccess = 0,
        eWrappingWaitingForData,
        eWrappingError,
    };

    virtual ~IOutputStream(){};

    virtual size_t getStreamIndex() const = 0;

    /**
     * @return The current duration of the stream, in seconds.
     */
    virtual float getStreamDuration() const = 0;

    /**
     * @return The current number of frame of the stream.
     */
    virtual size_t getNbFrames() const = 0;

    /**
     * @brief Wrap a packet of data
     * @return the wrapping status after wrapping
     * @see EWrappingStatus
    **/
    virtual EWrappingStatus wrap(const CodedData& data) = 0;
};
}

#endif