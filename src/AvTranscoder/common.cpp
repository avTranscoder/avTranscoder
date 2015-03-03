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

std::string getDescriptionFromErrorCode( const int code )
{
    char err[AV_ERROR_MAX_STRING_SIZE];
    av_strerror( code, err, sizeof(err) );
    return std::string( err );
}

void Logger::setLogLevel( const int level )
{
	av_log_set_level( level );
}

void Logger::log( int level, const std::string& msg )
{
	std::string avTranscoderMsg( "[avTranscoder - " );

	std::string levelStr;
	switch( level )
	{
		case AV_LOG_DEBUG:
			levelStr = "debug";
			break;
		case AV_LOG_INFO:
			levelStr = "info";
			break;
		case AV_LOG_WARNING:
			levelStr = "warning";
			break;
		case AV_LOG_ERROR:
			levelStr = "error";
			break;
		default:
			break;
	}

	avTranscoderMsg += levelStr;
	avTranscoderMsg += "] ";
	avTranscoderMsg += msg;
	avTranscoderMsg += "\n";
	av_log( NULL, level, avTranscoderMsg.c_str() );
}

}
