#ifndef _AV_TRANSCODER_PROFILE_UTIL_HPP_
#define _AV_TRANSCODER_PROFILE_UTIL_HPP_

#if defined ( __LINUX__ )

 #define DIRLIST_SEP_CHARS ":;"
 #define DIRSEP "/"
#include <dirent.h>

#elif defined ( __MACOS__ )

 #define DIRLIST_SEP_CHARS ";:"
 #define DIRSEP "/"
#include <dirent.h>

#elif defined ( __WINDOWS__ )

 #define DIRLIST_SEP_CHARS ";"
 #define DIRSEP "\\"

 // CINTERFACE needs to be declared if compiling with VC++
 #include <shlobj.h>
 #include <tchar.h>
 #ifndef _MSC_VER
	#define SHGFP_TYPE_CURRENT 0
 #endif

#endif

#include <string>
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
#if defined ( __WINDOWS__ )
	WIN32_FIND_DATA findData;
	HANDLE findHandle;

	findHandle = FindFirstFile( ( dir + "\\*" ).c_str(), &findData );
	if( findHandle == INVALID_HANDLE_VALUE )
	{
		return -1;
	}
	while(1)
	{
		const std::string filename( findData.cFileName );
		bool isdir = ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;
		if( ! isdir )
			files.push_back( filename );

		int rval = FindNextFile( findHandle, &findData );
		if( rval == 0 )
			break;
	}

#else
	DIR *dp;
	struct dirent *dirp;
	if( ( dp = opendir( dir.c_str() ) ) == NULL )
	{
		std::cerr << "Error(" << errno << ") opening " << dir << std::endl;
		return errno;
	}

	while( ( dirp = readdir( dp ) ) != NULL )
	{
		const std::string filename( dirp->d_name );
		if( filename == "." || filename == ".." )
			continue;
		files.push_back( filename );
	}
	closedir( dp );
#endif

	return 0;
}

}

#endif
