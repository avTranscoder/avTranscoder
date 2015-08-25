#ifndef _AV_TRANSCODER_IREADER_HPP
#define	_AV_TRANSCODER_IREADER_HPP

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/mediaProperty/StreamProperties.hpp>
#include <AvTranscoder/decoder/IDecoder.hpp>
#include <AvTranscoder/frame/Frame.hpp>
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
	 */
	IReader( const std::string& filename, const size_t streamIndex );

	/**
	 * @brief Get the existing InputFile and prepare to read the stream at the given index
	 * @note This constructor can improve performances when you create several readers from one InputFile.
	 */
	IReader( InputFile& inputFile, const size_t streamIndex );

	virtual ~IReader() = 0;

	/**
	 * @return Get next frame after decoding
	 */
	const char* readNextFrame();

	/**
	 * @return Get previous frame after decoding
	 */
	const char* readPrevFrame();

	/**
	 * @return Get indicated frame after decoding
	 */
	const char* readFrameAt( const size_t frame );

	/**
	 * @brief Print info of the stream read.
	 */
	virtual void printInfo();

protected:
	InputFile* _inputFile;
	const StreamProperties* _streamProperties;
	IDecoder* _decoder;

	Frame* _srcFrame;
	Frame* _dstFrame;

	ITransform* _transform;

	size_t _streamIndex;
	size_t _currentFrame;  ///< The current decoded frame.

	bool _inputFileAllocated;  ///< Is the InputFile hold by the class or not (depends on the constructor called)
};

}

#endif
