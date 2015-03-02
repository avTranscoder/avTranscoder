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

std::string AvExport getDescriptionFromErrorCode( const int code )
{
    char err[AV_ERROR_MAX_STRING_SIZE];
    av_strerror( code, err, sizeof(err) );
    return std::string( err );
}

}
