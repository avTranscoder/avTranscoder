#ifndef _AV_TRANSCODER_OUTPUT_STREAM_VIDEO_HPP_
#define _AV_TRANSCODER_OUTPUT_STREAM_VIDEO_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/pixfmt.h>
}

#include "OutputStream.hpp"

#include <string>
#include <vector>

class AVFormatContext;
class AVCodec;
class AVCodecContext;

namespace avtranscoder
{

class OutputStreamVideo : OutputStream
{
public:
	OutputStreamVideo();

	bool setup();

	void setWidth     ( const size_t w )  { width = w; }
	void setHeight    ( const size_t h )  { height = h; }
	void setComponents( const size_t c )  { components = c; }
	void setBitDepth  ( const size_t bd ) { bitDepth = bd; }

	/**
	 * @param[out] codecFrame blabla
	 */
	bool encodeFrame( const std::vector<unsigned char>& sourceImage, std::vector<unsigned char>& codedFrame );

private:
	AVCodec*         codec;
	AVCodecContext*  codecContext;

	size_t width;
	size_t height;
	size_t components;
	size_t bitDepth;
	PixelFormat pixelFormat;
};

}

#endif