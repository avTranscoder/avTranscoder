#include "Description.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/version.h>
#include <libavcodec/version.h>
#include <libswscale/version.h>
#ifdef AV_RESAMPLE_LIBRARY
	#include <libavresample/version.h>
#else
	#include <libswresample/version.h>
#endif
#include <libavformat/avformat.h>
}

#include <cstring>
#include <algorithm> //sort, unique

namespace avtranscoder
{

std::map< std::string, std::vector<size_t> > getVersion()
{
	std::map< std::string, std::vector<size_t> > version;

	std::vector<size_t> avutilVersion;
	avutilVersion.push_back( LIBAVUTIL_VERSION_MAJOR );
	avutilVersion.push_back( LIBAVUTIL_VERSION_MINOR );
	avutilVersion.push_back( LIBAVUTIL_VERSION_MICRO );

	std::vector<size_t> avformatVersion;
	avformatVersion.push_back( LIBAVFORMAT_VERSION_MAJOR );
	avformatVersion.push_back( LIBAVFORMAT_VERSION_MINOR );
	avformatVersion.push_back( LIBAVFORMAT_VERSION_MICRO );

	std::vector<size_t> avcodecVersion;
	avcodecVersion.push_back( LIBAVCODEC_VERSION_MAJOR );
	avcodecVersion.push_back( LIBAVCODEC_VERSION_MINOR );
	avcodecVersion.push_back( LIBAVCODEC_VERSION_MICRO );

	std::vector<size_t> swscaleVersion;
	swscaleVersion.push_back( LIBSWSCALE_VERSION_MAJOR );
	swscaleVersion.push_back( LIBSWSCALE_VERSION_MINOR );
	swscaleVersion.push_back( LIBSWSCALE_VERSION_MICRO );

#ifdef AV_RESAMPLE_LIBRARY
	std::vector<size_t> avresampleVersion;
	avresampleVersion.push_back( LIBAVRESAMPLE_VERSION_MAJOR );
	avresampleVersion.push_back( LIBAVRESAMPLE_VERSION_MINOR );
	avresampleVersion.push_back( LIBAVRESAMPLE_VERSION_MICRO );
#else
	std::vector<size_t> swresampleVersion;
	swresampleVersion.push_back( LIBSWRESAMPLE_VERSION_MAJOR );
	swresampleVersion.push_back( LIBSWRESAMPLE_VERSION_MINOR );
	swresampleVersion.push_back( LIBSWRESAMPLE_VERSION_MICRO );
#endif

	version[ "avutil" ] = avutilVersion;
	version[ "avformat" ] = avformatVersion;
	version[ "avcodec" ] = avcodecVersion;
	version[ "swscale" ] = swscaleVersion;
#ifdef AV_RESAMPLE_LIBRARY
	version[ "avresample" ] = avresampleVersion;
#else
	version[ "swresample" ] = swresampleVersion;
#endif

	return version;
}

std::string getLicence()
{
	std::string licence( avutil_license() );
	return licence;
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
