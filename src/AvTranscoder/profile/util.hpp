#ifndef _AV_TRANSCODER_PROFILE_UTIL_HPP_
#define _AV_TRANSCODER_PROFILE_UTIL_HPP_

#include <string>
#include <dirent.h>
#include <iostream>

namespace avtranscoder
{

void split( std::vector< std::string >& splitString, const std::string& inputString, const std::string& splitChars )
{
	char* part = strtok( const_cast<char*>( inputString.c_str() ), splitChars.c_str() );
	while( part != NULL )
	{
		splitString.push_back( std::string( part ) );
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

}

#endif
