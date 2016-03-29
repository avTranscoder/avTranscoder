#ifndef _AV_TRANSCODER_IREADER_HPP
#define _AV_TRANSCODER_IREADER_HPP

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/properties/StreamProperties.hpp>
#include <AvTranscoder/decoder/IDecoder.hpp>
#include <AvTranscoder/data/decoded/Frame.hpp>
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
     * @brief Create a new InputFile and prepare to read the stream at the given index
     * @param streamIndex by default read the first stream
     * @param channelIndex by default -1 (all channels of the stream)
     */
    IReader(const std::string& filename, const size_t streamIndex = 0, const int channelIndex = -1);

    /**
     * @brief Get the existing InputFile and prepare to read the stream at the given index
     * @note This constructor can improve performances when you create several readers from one InputFile.
     */
    IReader(InputFile& inputFile, const size_t streamIndex = 0, const int channelIndex = -1);

    virtual ~IReader() = 0;

    /**
     * @return Get next frame after decoding
     */
    Frame* readNextFrame();

    /**
     * @return Get previous frame after decoding
     */
    Frame* readPrevFrame();

    /**
     * @return Get indicated frame after decoding
     */
    Frame* readFrameAt(const size_t frame);

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
    InputFile* _inputFile;
    const StreamProperties* _streamProperties;
    IDecoder* _decoder;
    IDecoder* _generator;

    Frame* _srcFrame;
    Frame* _dstFrame;

    ITransform* _transform;

    size_t _streamIndex;
    int _channelIndex;

private:
    int _currentFrame;        ///< The current decoded frame.
    bool _inputFileAllocated; ///< Does the InputFile is held by the class or not (depends on the constructor called)
    bool _continueWithGenerator;  ///< If there is no more data to decode, complete with generated data
};
}

#endif
