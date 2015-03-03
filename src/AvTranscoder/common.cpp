#include "common.hpp"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/error.h>
}

namespace avtranscoder
{

void preloadCodecsAndFormats()
{
	av_register_all();
}

std::string getDescriptionFromErrorCode( const int code )
{
    char err[AV_ERROR_MAX_STRING_SIZE];
    av_strerror( code, err, sizeof(err) );
    return std::string( err );
}

}
