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
#include <algorithm> //sort, unique

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
			// parse extensions
			std::string exts = std::string( iFormat->extensions );
			char* ext = strtok( const_cast<char*>( exts.c_str() ), "," );
			while( ext != NULL )
			{
				extensions.push_back( std::string( ext ) );
				ext = strtok( NULL, "," );
			}
			
			// parse name (name's format defines (in general) extensions )
			// don't need to do it in recent LibAV/FFMpeg versions
			exts = std::string( iFormat->name );
			ext = strtok( const_cast<char*>( exts.c_str() ), "," );
			while( ext != NULL )
			{
				extensions.push_back( std::string( ext ) );
				ext = strtok( NULL, "," );
			}
		}
	}
	// sort
	std::sort( extensions.begin(), extensions.end() ); 
	// suppress duplicates
	std::vector<std::string>::iterator last = std::unique( extensions.begin(), extensions.end() );
	extensions.erase( last, extensions.end() );
	
	return extensions;
}

std::vector<std::string> getOutputExtensions()
{
	av_register_all();
	std::vector<std::string> extensions;
	AVOutputFormat* oFormat = NULL;

	while( ( oFormat = av_oformat_next( oFormat ) ) )
	{
		if( oFormat->extensions != NULL )
		{			
			// parse extensions
			std::string exts = std::string( oFormat->extensions );
			char* ext = strtok( const_cast<char*>( exts.c_str() ), "," );
			while( ext != NULL )
			{
				extensions.push_back( std::string( ext ) );
				ext = strtok( NULL, "," );
			}
			
			// parse name (name's format defines (in general) extensions )
			// don't need to do it in recent LibAV/FFMpeg versions
			exts = std::string( oFormat->name );
			ext = strtok( const_cast<char*>( exts.c_str() ), "," );
			while( ext != NULL )
			{
				extensions.push_back( std::string( ext ) );
				ext = strtok( NULL, "," );
			}
		}
	}
	// sort
	std::sort( extensions.begin(), extensions.end() ); 
	// suppress duplicates
	std::vector<std::string>::iterator last = std::unique( extensions.begin(), extensions.end() );
	extensions.erase( last, extensions.end() );
	
	return extensions;
}

}

#endif
