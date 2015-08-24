#ifndef _AV_TRANSCODER_IREADER_HPP
#define	_AV_TRANSCODER_IREADER_HPP

#include <AvTranscoder/common.hpp>

namespace avtranscoder
{

class AvExport IReader
{
public:
	IReader()
	: _currentFrame( 0 )
	{}

	/**
	 * @return Get next frame after decoding
	 */
	virtual const char* readNextFrame() = 0;

	/**
	 * @return Get previous frame after decoding
	 */
	virtual const char* readPrevFrame() = 0;

	/**
	 * @return Get indicated frame after decoding
	 */
	virtual const char* readFrameAt( const size_t frame ) = 0;

	/**
	 * @brief Print info of the stream read.
	 */
	virtual void printInfo() = 0;

protected:
	size_t _currentFrame;
};

}

#endif
