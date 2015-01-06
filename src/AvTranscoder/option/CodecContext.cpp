#include "CodecContext.hpp"

namespace avtranscoder
{

CodecContext::CodecContext( AVCodecContext& avCodecContext, int req_flags )
	: Context( &avCodecContext, req_flags )
{}

}
