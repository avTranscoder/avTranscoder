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
#include <libavformat/version.h>
#include <libavcodec/version.h>
#include <libavcodec/avcodec.h>
}

#include <string>
#include <cstring>
#include <vector>

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

struct Ratio
{
	size_t num;
	size_t den;
};

void split( std::vector< std::string >& splitedString, const std::string& inputString, const std::string& splitChars = ";" );

int getFilesInDir( std::string dir, std::vector< std::string > &files );

}

#endif
