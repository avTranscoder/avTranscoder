#ifndef _AV_TRANSCODER_COMMON_HPP_
#define _AV_TRANSCODER_COMMON_HPP_

#include <string>

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

namespace avtranscoder
{

#define MAX_SWS_PLANE 4

struct Ratio
{
	size_t num;
	size_t den;
};

}

#endif
