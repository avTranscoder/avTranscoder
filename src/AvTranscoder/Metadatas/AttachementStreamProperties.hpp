#ifndef _AV_TRANSCODER_ATTACHEMENT_STREAM_PROPERTIES_HPP_
#define _AV_TRANSCODER_ATTACHEMENT_STREAM_PROPERTIES_HPP_

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

AttachementProperties attachementStreamInfo( const AVFormatContext* formatContext, const size_t index )
{
	AttachementProperties ap;
	ap.streamId      = index;
	return ap;
}

}

#endif