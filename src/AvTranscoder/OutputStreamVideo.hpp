#ifndef _AV_TRANSCODER_OUTPUT_STREAM_VIDEO_HPP_
#define _AV_TRANSCODER_OUTPUT_STREAM_VIDEO_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/pixfmt.h>
}

#include "OutputStream.hpp"

#include "DatasStructures/Image.hpp"
#include "DatasStructures/VideoStream.hpp"

#include <string>
#include <vector>

class AVFormatContext;
class AVCodecContext;

namespace avtranscoder
{

class OutputStreamVideo : public OutputStream
{
public:
	OutputStreamVideo();

	bool setup();

	void setVideoDesc( const VideoStream& videoStream );

	/**
	 * @param[out] codecFrame blabla
	 */
	bool encodeFrame( const Image& sourceImage, Image& codedFrame );

	VideoStream& getVideoDesc() { return m_videoDesc; }

private:
	VideoStream m_videoDesc;
};

}

#endif