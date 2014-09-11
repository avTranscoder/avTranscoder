#ifndef _AV_TRANSCODER_SUBTITLE_STREAM_PROPERTIES_HPP_
#define _AV_TRANSCODER_SUBTITLE_STREAM_PROPERTIES_HPP_

class AVFormatContext;

namespace avtranscoder
{

SubtitleProperties subtitleStreamInfo( const AVFormatContext* formatContext, const size_t index )
{
	SubtitleProperties sp;
	sp.streamId = index;

	return sp;
}

}

#endif