#ifndef _AV_TRANSCODER_COMMON_HPP_
#define _AV_TRANSCODER_COMMON_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#ifndef INT64_C
	#define INT64_C(c) (c ## LL)
	#define UINT64_C(c) (c ## ULL)
#endif
#include <libavcodec/version.h>
#include <libavutil/error.h>
#include <libavutil/rational.h>
#include <libavutil/log.h>
}

#include <string>
#include <cstring>
#include <sstream>

#ifdef SWIG
 #define AvExport
#else
#if ( defined( WIN32 ) || defined( WIN64 ) || defined( _WIN32 ) || defined( _WIN64 ) || defined( __WINDOWS__ ) || defined( __TOS_WIN__ ) || defined( __WIN32__ ) )
 #define AvExport __declspec( dllexport )
#elif defined( __GNUC__ )     // Add compiler definition here...
 #if __GNUC__ - 0 > 3 || ( __GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 > 2 )
  #define AvExport __attribute__ ( ( visibility( "default" ) ) )
 #else
  #define AvExport
  #warning "AvExport not set because of a too old gcc version. The plug-in may not compile with the option -fvisible=hidden."
 #endif
#else
 #error "AvExport not defined for this compiler..."
#endif
#endif

#ifndef AV_ERROR_MAX_STRING_SIZE
 #define AV_ERROR_MAX_STRING_SIZE 64
#endif

#ifndef SWIG
#if LIBAVCODEC_VERSION_MAJOR < 54
 #define AVPixelFormat PixelFormat
 #define AV_PIX_FMT_NONE PIX_FMT_NONE
 #define AVCodecID CodecID
 #define AV_CODEC_ID_NONE CODEC_ID_NONE
#endif
#endif

namespace avtranscoder
{

#define MAX_SWS_PLANE 4

typedef AVRational Rational;

/// Register all the codecs and formats which are enabled at configuration time.
void AvExport preloadCodecsAndFormats();

#define LOG_DEBUG( ... ) { std::stringstream os; os << __VA_ARGS__; Logger::log( AV_LOG_DEBUG, "debug", os.str() ); }
#define LOG_INFO( ... ) { std::stringstream os; os << __VA_ARGS__; Logger::log( AV_LOG_INFO, "info", os.str() ); }
#define LOG_WARN( ... ) { std::stringstream os; os << __VA_ARGS__; Logger::log( AV_LOG_WARNING, "warning", os.str() ); }
#define LOG_ERROR( ... ) { std::stringstream os; os << __VA_ARGS__; Logger::log( AV_LOG_ERROR, "error", os.str() ); }

/// Logger class which contains static functions to use ffmpeg/libav log system
class AvExport Logger
{
public:
	/**
	 * @brief Set the log level of ffmpeg/libav.
	 * @param level: refer to define AV_LOG_xxx (from AV_LOG_QUIET to AV_LOG_DEBUG)
	 * @see SWIG interface avLogLevel.i
	 */
	static void setLogLevel( const int level );

	/**
	 * @brief Log with the ffmpeg/libav log system
	 * @note use define LOG_* to log at DEBUG/INFO/WARN/ERROR level
	 * @param msg: the message will be prefixed by '[avTranscoder - <level>]'
	 * @param msg: the message will be suffixed by '\n'
	 */
	static void log( int level, const std::string& levelStr, const std::string& msg );
};

}

#endif
