#ifndef _MEDIA_MANAGER_IO_SUBTITLE_STREAM_PROPERTIES_HPP_
#define _MEDIA_MANAGER_IO_SUBTITLE_STREAM_PROPERTIES_HPP_

class AVFormatContext;

namespace mediamanager
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