#include "common.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
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

std::string getFormat( const std::string& filename )
{
	std::string format( "" );
	
	AVOutputFormat* avOutputFormat = av_guess_format( NULL, filename.c_str(), NULL);
	if( avOutputFormat )
	{
		if( avOutputFormat->name )
			format = std::string( avOutputFormat->name );
	}
	
	return format;
}

bool matchFormat( const std::string& format, const std::string& filename )
{
	AVOutputFormat* avOutputFormat = av_guess_format( format.c_str(), filename.c_str(), NULL);
	return avOutputFormat != NULL;
}


}
