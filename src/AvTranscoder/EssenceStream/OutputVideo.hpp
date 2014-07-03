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
#include <AvTranscoder/DatasStructures/Image.hpp>

#include <AvTranscoder/Profile.hpp>

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

	void setProfile( Profile::ProfileDesc& desc, const avtranscoder::ImageDesc& imageDesc );
	
	VideoDesc& getVideoDesc() { return _videoDesc; }

private:
	VideoDesc _videoDesc;
};

}

#endif