#ifndef _AV_TRANSCODER_SUBTITLE_STREAM_PROPERTIES_HPP_
#define _AV_TRANSCODER_SUBTITLE_STREAM_PROPERTIES_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

namespace avtranscoder
{

SubtitleProperties subtitleStreamInfo( const AVFormatContext* formatContext, const size_t index )
{
	SubtitleProperties sp;
	sp.streamId      = index;

	return sp;
}

}

#endif