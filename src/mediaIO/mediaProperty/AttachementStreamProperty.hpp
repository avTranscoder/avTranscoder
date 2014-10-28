#ifndef _MEDIA_MANAGER_IO_ATTACHEMENT_STREAM_PROPERTIES_HPP_
#define _MEDIA_MANAGER_IO_ATTACHEMENT_STREAM_PROPERTIES_HPP_

class AVFormatContext;

namespace mediamanager
{
namespace mediaio
{

AttachementProperties attachementStreamInfo( const AVFormatContext* formatContext, const size_t index )
{
	AttachementProperties ap;
	ap.streamId = index;
	return ap;
}

}
}

#endif