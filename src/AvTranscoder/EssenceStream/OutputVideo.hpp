#ifndef _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_VIDEO_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/pixfmt.h>
}

#include "OutputEssence.hpp"

#include <AvTranscoder/DatasStructures/VideoDesc.hpp>
#include <AvTranscoder/DatasStructures/DataStreamDesc.hpp>

#include <string>
#include <vector>

class AVFormatContext;
class AVCodecContext;

namespace avtranscoder
{

class AvExport OutputVideo : public OutputEssence
{
public:
	OutputVideo();

	bool setup();

	//void setVideoDesc( const VideoDesc& videoDesc );

	/**
	 * @param[out] codedFrame blabla
	 */
	bool encodeFrame( const Frame& sourceFrame, DataStream& codedFrame );

	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( DataStream& codedFrame );

	VideoDesc& getVideoDesc() { return _videoDesc; }

	void setProfile( const std::string& profile );

private:
	VideoDesc _videoDesc;
};

}

#endif