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
#include <cstring>

namespace avtranscoder
{

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
	av_register_all();
	std::vector<std::string> extensions;
	AVInputFormat* iFormat = NULL;

	while( ( iFormat = av_iformat_next( iFormat ) ) )
	{
		if( iFormat->extensions != NULL )
		{
			char* ext = const_cast<char*>( iFormat->extensions );

			while( ext != NULL )
			{
				extensions.push_back( std::string( ext ) );
				ext = strtok( NULL, "," );
			}
		}
	}
	return extensions;
}

std::vector<std::string> getOutputExtensions()
{
	std::vector<std::string> extensions;

	return extensions;
}

}

#endif
