#ifndef _AV_TRANSCODER_UNKNOWN_STREAM_PROPERTIES_HPP_
#define _AV_TRANSCODER_UNKNOWN_STREAM_PROPERTIES_HPP_

class AVFormatContext;

namespace avtranscoder
{

UnknownProperties unknownStreamInfo( const AVFormatContext* formatContext, const size_t index )
{
	UnknownProperties up;
	up.streamId = index;

	return up;
}

}

#endif