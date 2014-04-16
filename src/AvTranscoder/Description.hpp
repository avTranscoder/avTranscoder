#ifndef _AV_TRANSCODER_DESCRIPTION_HPP_
#define _AV_TRANSCODER_DESCRIPTION_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavformat/avformat.h>
#include <libavutil/avstring.h>
}

#include <vector>

std::vector<size_t> getVersion()
{
	std::vector<size_t> version;
	version.push_back( 0 );
	version.push_back( 0 );
	version.push_back( 0 );
	// avutil_version
	// avutil_license
	return version;
}

std::vector<std::string> getInputExtensions()
{
	av_register_all();  // Warning: should be called only once
	std::vector<std::string> extensions;
	AVInputFormat* iFormat = av_iformat_next( NULL );
	while( iFormat != NULL )
	{
		if( iFormat->extensions != NULL )
		{
			const char* exts = iFormat->extensions;
			while( 1 )
			{
				char* saveptr = NULL;
				char* ext = av_strtok( const_cast<char*>( exts ), ",", &saveptr );
				if( ! ext || ! saveptr )
				{
					break;
				}
				extensions.push_back( std::string( ext ) );
			}
		}
		iFormat = av_iformat_next( iFormat );
	}
	return extensions;
}

std::vector<std::string> getOutputExtensions()
{
	std::vector<std::string> extensions;

	return extensions;
}



#endif

