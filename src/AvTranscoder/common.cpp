#include "common.hpp"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/log.h>
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

void Logger::debug( const std::string msg )
{
	log( AV_LOG_DEBUG, "debug", msg );
}

void Logger::info( const std::string msg )
{
	log( AV_LOG_INFO, "info", msg );
}

void Logger::warn( const std::string msg )
{
	log( AV_LOG_WARNING, "warning", msg );
}

void Logger::error( const std::string msg )
{
	log( AV_LOG_ERROR, "error", msg );
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
