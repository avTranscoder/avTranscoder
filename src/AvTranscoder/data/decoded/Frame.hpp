#ifndef _AV_TRANSCODER_FRAME_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_FRAME_HPP_

#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavutil/frame.h>
}

struct AVStream;

namespace avtranscoder
{

/**
 * @brief This class describes decoded (raw) audio or video data.
 */
class AvExport Frame
{
public:
    /**
     * @brief Allocate an empty frame.
     */
    Frame();

    /**
     * @brief Copy properties and reference data of the other frame
     */
    Frame(const Frame& otherFrame);

    virtual ~Frame();

    /**
     * @brief Get all the data of the frame.
     */
    unsigned char** getData() { return _frame->data; }

    /**
     * @brief Returns the size in byte.
     * For video, size in bytes of each picture line.
     * For audio, size in bytes of each plane.
     * @note For audio, only linesize[0] may be set. For planar audio, each channel
     * plane must be the same size.
     */
    int* getLineSize() const { return _frame->linesize; }

    /**
    * @brief Copy the data of the given Frame.
    */
    void copyData(const Frame& frameToRef);

    /**
     * @brief Copy all the fields that do not affect the data layout in the buffers.
     */
    void copyProperties(const Frame& otherFrame);

    /**
     * @brief Copy frame properties and create a new reference to data of the given frame.
     * @warning This method allocates new data that will be freed only by calling the destructor of the referenced frame.
     */
    void refFrame(const Frame& otherFrame);

#ifndef SWIG
    void refAVStream(const AVStream& avStream) { _avStream = &avStream; }
#endif

    /**
     * @brief Unreference all the buffers referenced by frame and reset the frame fields.
     */
    void clear();

    /**
     * @return If it corresponds to a valid audio frame.
     * @see AudioFrame
     */
    bool isAudioFrame() const;

    /**
     * @return If it corresponds to a valid video frame.
     * @see VideoFrame
     */
    bool isVideoFrame() const;

#ifndef SWIG
    /**
     * @return the AVStream which contains the decoded packet
     * @note may be NULL in case of generated packets
     */
    const AVStream* getAVStream() const { return _avStream; }

    AVFrame& getAVFrame() { return *_frame; }
    const AVFrame& getAVFrame() const { return *_frame; }

    const unsigned char** getData() const { return const_cast<const unsigned char**>(_frame->data); }
#endif

private:
    void allocateAVFrame();

protected:
    AVFrame* _frame;

    // Stream which contains the decoded packet
    const AVStream* _avStream; //< Has link (no ownership)
};
}

#endif
