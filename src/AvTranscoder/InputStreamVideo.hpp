#ifndef _AV_TRANSCODER_INPUT_STREAM_VIDEO_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_VIDEO_HPP_

#include "InputStream.hpp"
#include "DatasStructures/Image.hpp"

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

	Image& readNextFrame( Image& frameBuffer );

private:
	AVFormatContext* formatContext;
	AVCodec*         codec;
	AVCodecContext*  codecContext;

	int    selectedStream;
};

}

#endif