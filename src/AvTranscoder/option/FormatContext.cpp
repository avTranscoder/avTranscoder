#include "FormatContext.hpp"

namespace avtranscoder
{

FormatContext::FormatContext( AVFormatContext& avFormatContext, int req_flags )
	: Context( &avFormatContext, req_flags )
{}

}
