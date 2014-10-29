#include "common.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
}

#include <dirent.h>
#include <iostream>

namespace mediaengine
{
namespace mediacore
{

void split( std::vector< std::string >& splitedString, const std::string& inputString, const std::string& splitChars )
{
	char* part = strtok( const_cast<char*>( inputString.c_str() ), splitChars.c_str() );
	while( part != NULL )
	{
		splitedString.push_back( std::string( part ) );
		part = strtok( NULL, splitChars.c_str() );
	}
}

int getFilesInDir( const std::string& dir, std::vector< std::string >& files )
{
	DIR *dp;
	struct dirent *dirp;
	if( ( dp  = opendir( dir.c_str() ) ) == NULL )
	{
		std::cerr << "Error(" << errno << ") opening " << dir << std::endl;
		return errno;
	}

	while( ( dirp = readdir( dp ) ) != NULL )
	{
		std::string filename( dirp->d_name );
		if( filename == "." || filename == ".." )
			continue;
		files.push_back( filename );
	}
	closedir( dp );
	return 0;
}

std::string getFormat( const std::string& filename )
{
	std::string format( "" );
	
	av_register_all();
	AVOutputFormat* avOutputFormat = av_guess_format( NULL, filename.c_str(), NULL);
	if( avOutputFormat )
	{
		if( avOutputFormat->name )
			format = std::string( avOutputFormat->name );
	}
	
	return format;
}

bool matchFormat( const std::string& format, const std::string& filename )
{
	AVOutputFormat* avOutputFormat = av_guess_format( format.c_str(), filename.c_str(), NULL);
	return avOutputFormat != NULL;
}

}
}
