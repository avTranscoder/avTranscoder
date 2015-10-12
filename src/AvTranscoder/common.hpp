#ifndef _AV_TRANSCODER_COMMON_HPP_
#define _AV_TRANSCODER_COMMON_HPP_

#define AVTRANSCODER_VERSION_MAJOR 0
#define AVTRANSCODER_VERSION_MINOR 5
#define AVTRANSCODER_VERSION_MICRO 10

#include <AvTranscoder/system.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#ifndef INT64_C
	#define INT64_C(c) (c ## LL)
	#define UINT64_C(c) (c ## ULL)
#endif
#include <libavcodec/version.h>
#include <libavutil/rational.h>
}

#include <string>

#ifdef SWIG
 #define AvExport
#else
#if defined( __WINDOWS__ )
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

#ifndef SWIG
#if LIBAVCODEC_VERSION_MAJOR < 54
 #define AVPixelFormat PixelFormat
 #define AV_PIX_FMT_NONE PIX_FMT_NONE
 #define AVCodecID CodecID
 #define AV_CODEC_ID_NONE CODEC_ID_NONE
#endif
#endif

#include <AvTranscoder/log.hpp>

namespace avtranscoder
{

typedef AVRational Rational;

/// Register all the codecs and formats which are enabled at configuration time.
void AvExport preloadCodecsAndFormats();

/// Get the string description corresponding to the error code provided by ffmpeg/libav
std::string AvExport getDescriptionFromErrorCode( const int code );

}

#endif
