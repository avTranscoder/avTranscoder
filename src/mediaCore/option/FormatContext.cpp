#include "FormatContext.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

namespace mediaengine
{
namespace mediacore
{

FormatContext::FormatContext( int req_flags )
	: _avFormatContext( NULL )
{
	_avFormatContext = avformat_alloc_context();
	loadOptions( _avFormatContext, req_flags );
}

FormatContext::~FormatContext()
{
	avformat_free_context( _avFormatContext );
}

}
}
