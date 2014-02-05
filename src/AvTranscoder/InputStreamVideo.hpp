#ifndef _AV_TRANSCODER_INPUT_STREAM_VIDEO_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_VIDEO_HPP_

#include "InputStream.hpp"

#include <vector>

class AVFormatContext;
class AVCodec;
class AVCodecContext;

namespace avtranscoder
{

class InputStreamVideo : public InputStream
{
public:
	InputStreamVideo();

	bool setup( const std::string& filename, const size_t streamIndex );

	size_t getWidth()      const { return width; }
	size_t getHeight()     const { return height; }
	size_t getComponents() const { return components; }
	size_t getBitDepth()   const { return bitDepth; }

	std::vector<unsigned char>& readNextFrame( std::vector<unsigned char>& frameBuffer );

private:
	AVFormatContext* formatContext;
	AVCodec*         codec;
	AVCodecContext*  codecContext;

	int    selectedStream;
	size_t width;
	size_t height;
	size_t components;
	size_t bitDepth;
};

}

#endif