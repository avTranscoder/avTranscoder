#include "common.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

namespace avtranscoder
{

void preloadCodecsAndFormats()
{
	av_register_all();
}

void setLogLevel( const int level )
{
	av_log_set_level( level );
}

}
