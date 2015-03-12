#ifndef _AV_TRANSCODER_LOG_HPP
#define	_AV_TRANSCODER_LOG_HPP

#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavutil/log.h>
}

#include <sstream>
#include <fstream>

namespace avtranscoder
{

#define LOG_FILE "avtranscoder.log"
#define LOG_DEBUG( ... ) { std::stringstream os; os << __VA_ARGS__; Logger::log( AV_LOG_DEBUG, os.str() ); }
#define LOG_INFO( ... ) { std::stringstream os; os << __VA_ARGS__; Logger::log( AV_LOG_INFO, os.str() ); }
#define LOG_WARN( ... ) { std::stringstream os; os << __VA_ARGS__; Logger::log( AV_LOG_WARNING, os.str() ); }
#define LOG_ERROR( ... ) { std::stringstream os; os << __VA_ARGS__; Logger::log( AV_LOG_ERROR, os.str() ); }

/// Logger class which contains static functions to use ffmpeg/libav log system
class AvExport Logger
{
public:
	/**
	 * @brief Set the log level of ffmpeg/libav.
	 * @param level: refer to define AV_LOG_xxx (from AV_LOG_QUIET to AV_LOG_DEBUG)
	 * @note By default AV_LOG_INFO
	 * @see SWIG interface avLogLevel.i
	 */
	static void setLogLevel( const int level );

	/**
	 * @brief Log with the ffmpeg/libav log system
	 * @note you can use macro LOG_* to log at DEBUG/INFO/WARN/ERROR level
	 * @param msg: the message will be prefixed by '[avTranscoder - <level>]'
	 * @param msg: the message will be suffixed by '\n'
	 */
	static void log( const int level, const std::string& msg );

	/**
	 * @brief Log ffmpeg/libav and avtranscoder informations in a text file.
	 * @note log filename is avtranscoder.log
	 */
	static void logInFile();
};

}

#endif
