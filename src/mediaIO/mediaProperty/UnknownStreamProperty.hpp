#ifndef _MEDIA_ENGINE_IO_UNKNOWN_STREAM_PROPERTIES_HPP_
#define _MEDIA_ENGINE_IO_UNKNOWN_STREAM_PROPERTIES_HPP_

class AVFormatContext;

namespace mediaengine
{
namespace mediaio
{

UnknownProperties unknownStreamInfo( const AVFormatContext* formatContext, const size_t index )
{
	UnknownProperties up;
	up.streamId = index;

	return up;
}

}
}

#endif