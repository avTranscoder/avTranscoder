#ifndef _MEDIA_ENGINE_IO_SUBTITLE_STREAM_PROPERTIES_HPP_
#define _MEDIA_ENGINE_IO_SUBTITLE_STREAM_PROPERTIES_HPP_

class AVFormatContext;

namespace mediaengine
{
namespace mediaio
{

SubtitleProperties subtitleStreamInfo( const AVFormatContext* formatContext, const size_t index )
{
	SubtitleProperties sp;
	sp.streamId = index;

	return sp;
}

}
}

#endif