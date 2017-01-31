#ifndef _AV_TRANSCODER_IREADER_HPP
#define _AV_TRANSCODER_IREADER_HPP

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/transcoder/InputStreamDesc.hpp>
#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/properties/StreamProperties.hpp>
#include <AvTranscoder/decoder/IDecoder.hpp>
#include <AvTranscoder/data/decoded/IFrame.hpp>
#include <AvTranscoder/transform/ITransform.hpp>

namespace avtranscoder
{

/**
 * @brief Based class to read a stream.
 */
class AvExport IReader
{
public:
    /**
     * @brief Prepare to read the given input.
     * @param inputDesc: the description of the input to read.
     */
    IReader(const InputStreamDesc& inputDesc);

    virtual ~IReader() = 0;

    /**
     * @return Get next frame after decoding
     * @see readFrameAt
     */
    IFrame* readNextFrame();

    /**
     * @return Get previous frame after decoding
     * @see readFrameAt
     */
    IFrame* readPrevFrame();

    /**
     * @return Get indicated frame after decoding
     * @warn Returns NULL if there is no more frame to read.
     * @see continueWithGenerator
     */
    IFrame* readFrameAt(const size_t frame);

    /**
     * @brief Get the properties of the source stream read.
     */
    const StreamProperties* getSourceProperties() const { return _streamProperties; }

    /**
     * @brief Set the reader state to generate data (ie silence or black) when there is no more data to decode.
     * @note By default, the reader returns an empty frame.
     */
    void continueWithGenerator(const bool continueWithGenerator = true) { _continueWithGenerator = continueWithGenerator; }

protected:
    const InputStreamDesc _inputDesc;
    InputFile* _inputFile;
    const StreamProperties* _streamProperties;
    IDecoder* _decoder;
    IDecoder* _generator;
    IDecoder* _currentDecoder; ///< Link to _inputDecoder or _generator

    IFrame* _srcFrame;
    IFrame* _dstFrame;

    ITransform* _transform;

private:
    int _currentFrame;           ///< The current decoded frame.
    bool _continueWithGenerator; ///< If there is no more data to decode, complete with generated data
};
}

#endif
