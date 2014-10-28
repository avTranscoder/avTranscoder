#include "CodecContext.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/mem.h>
}

namespace mediamanager
{
namespace mediacore
{

CodecContext::CodecContext( int req_flags )
	: _avCodecContext( NULL )
{
	_avCodecContext = avcodec_alloc_context3( NULL );
	loadOptions( _avCodecContext, req_flags );
}

CodecContext::~CodecContext()
{
	avcodec_close( _avCodecContext );
	av_free( _avCodecContext );
}

}
}
