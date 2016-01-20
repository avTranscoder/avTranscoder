#ifndef _AV_TRANSCODER_FRAME_CODEDDATA_HPP_
#define _AV_TRANSCODER_FRAME_CODEDDATA_HPP_

#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
}

struct AVStream;

namespace avtranscoder
{

/**
 * @brief This class describes coded data.
 */
class AvExport CodedData
{
public:
    /// Create an empty data buffer
    CodedData();

    /// Create a data buffer with a the given size
    CodedData(const size_t dataSize);

#ifndef SWIG
    /// Create a data buffer from the given AVPAcket (copy data of given packet)
    CodedData(const AVPacket& avPacket);
#endif

    /// Override copy constructor in order to copy AVPacket data
    CodedData(const CodedData& other);

    /// Override operator = in order to copy AVPacket data
    CodedData& operator=(const CodedData& other);

    /// Free buffer of data
    ~CodedData();

#ifndef SWIG
    void refAVStream(const AVStream& avStream) { _avStream = &avStream; }
#endif
    /// Resize data buffer
    void resize(const size_t newSize);

    ///@{
    /// Ref to external data buffer
    void refData(CodedData& frame);
    void refData(unsigned char* buffer, const size_t size);
    ///@}

    /// Copy external data buffer
    void copyData(unsigned char* buffer, const size_t size);

    /**
     * @brief Resize the buffer with the given size, and copy the given value
     * @note Use this function to check if we can modify the buffer
     */
    void assign(const size_t size, const int value);

    /// Clear existing data and set size to 0
    void clear();

    unsigned char* getData() { return _packet.data; }
#ifndef SWIG
    const unsigned char* getData() const { return _packet.data; }
#endif

    size_t getSize() const { return _packet.size; }

#ifndef SWIG
    /**
     * @return the AVStream which contains the packet
     * @note may be NULL in case of generated packets
     */
    const AVStream* getAVStream() const { return _avStream; }
    AVPacket& getAVPacket() { return _packet; }
    const AVPacket& getAVPacket() const { return _packet; }
#endif

private:
    void initAVPacket();
    void copyAVPacket(const AVPacket& avPacket);

private:
    AVPacket _packet;

    // Stream which contains the packet
    const AVStream* _avStream; //< Has link (no ownership)
};
}

#endif
