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
     * @warn This only allocates the AVFrame itself, not the data buffers.
     */
    Frame();

    //@{
    // @brief Copy properties and reference data of the other frame.
    Frame(const Frame& otherFrame);
    void operator=(const Frame& otherFrame);
    //@}

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
     * @return Size of the corresponding packet containing the compressed frame (in bytes)
     * @warning returns a negative value if the size is unknown
     */
    int getEncodedSize() const;

    /**
     * @brief Copy the data of the given Frame.
     * This function does not allocate anything: the current frame must be already initialized and
     * allocated with the same parameters as the given frame, to be ready for memcpy instructions.
     * @note It copies the frame data (i.e. the contents of the data / extended data arrays), not any other properties.
     * @see copyProperties
     */
    void copyData(const Frame& frameToRef);

    /**
     * @brief Copy all the fields that do not affect the data layout in the buffers.
     */
    void copyProperties(const Frame& otherFrame);

    /**
     * @brief If the data buffer of the frame refers to data allocated by an other frame.
     */
    bool isRefCounted() const;

    /**
     * @brief Copy frame properties and create a new reference to data of the given frame.
     * @warning This method allocates new data that will be freed by calling clear method or the destructor of the referenced frame.
     * @see clear
     */
    void refFrame(const Frame& otherFrame);

    /**
     * @brief Unreference all the buffers referenced by frame and reset the frame fields.
     */
    void unrefFrame();

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
