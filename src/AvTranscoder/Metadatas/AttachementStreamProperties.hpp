#ifndef _AV_TRANSCODER_ATTACHEMENT_STREAM_PROPERTIES_HPP_
#define _AV_TRANSCODER_ATTACHEMENT_STREAM_PROPERTIES_HPP_

class AVFormatContext;

namespace avtranscoder
{

AttachementProperties attachementStreamInfo( const AVFormatContext* formatContext, const size_t index )
{
	AttachementProperties ap;
	ap.streamId      = index;
	return ap;
}

}

#endif