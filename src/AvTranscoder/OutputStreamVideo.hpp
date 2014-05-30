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
#include "DatasStructures/VideoDesc.hpp"
#include "DatasStructures/DataStreamDesc.hpp"

#include <string>
#include <vector>

class AVFormatContext;
class AVCodecContext;

namespace avtranscoder
{

class AvExport OutputStreamVideo : public OutputStream
{
public:
	OutputStreamVideo();

	bool setup();

	//void setVideoDesc( const VideoDesc& videoDesc );

	/**
	 * @param[out] codecFrame blabla
	 */
	bool encodeFrame( const Image& sourceImage, DataStream& codedFrame );

	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( DataStream& codedFrame );

	VideoDesc& getVideoDesc() { return m_videoDesc; }

	void setProfile( const std::string& profile );

private:
	VideoDesc m_videoDesc;
};

}

#endif