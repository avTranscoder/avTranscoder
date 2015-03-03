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

void Logger::setLogLevel( const int level )
{
	av_log_set_level( level );
}

void Logger::log( int level, const std::string& levelStr, const std::string& msg )
{
	std::string avTranscoderMsg( "[avTranscoder - " );
	avTranscoderMsg += levelStr;
	avTranscoderMsg += "] ";
	avTranscoderMsg += msg;
	avTranscoderMsg += "\n";
	av_log( NULL, level, avTranscoderMsg.c_str() );
}

}
