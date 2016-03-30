#ifndef _AV_TRANSCODER_FRAME_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_FRAME_HPP_

#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavutil/frame.h>
}

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
     * @brief Ref to data of the given Frame.
     * @note This function does not allocate anything.
     * The current Frame must be already initialized and allocated with the same parameters as frameToRef.
     */
    void refData(const Frame& frameToRef);

    /**
     * @brief Copy all the fields that do not affect the data layout in the buffers.
     */
    void copyProperties(const Frame& otherFrame);

    /**
     * @brief Copy frame properties and create a new reference to data of the given frame.
     * @warning This method allocates new data that will be freed only by calling the destructor of the referenced frame.
     */
    void refFrame(const Frame& otherFrame);

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
    AVFrame& getAVFrame() { return *_frame; }
    const AVFrame& getAVFrame() const { return *_frame; }
    const unsigned char** getData() const { return const_cast<const unsigned char**>(_frame->data); }
#endif

private:
    void allocateAVFrame();

protected:
    AVFrame* _frame;
};
}

#endif
