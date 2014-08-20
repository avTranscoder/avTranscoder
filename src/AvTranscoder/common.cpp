#include "common.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
}

#include <dirent.h>
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace avtranscoder
{

void ParamSet::set( const std::string& key, const std::string& flag, const bool enable )
{
	int error = 0;
	int64_t optVal;
	
	const AVOption* flagOpt = av_opt_find( _objContext, flag.c_str(), key.c_str(), 0, 0 );

	if( ! flagOpt )
	{
		throw std::runtime_error( "unknown flag " + flag );
	}

	error = av_opt_get_int( _objContext, key.c_str(), AV_OPT_SEARCH_CHILDREN, &optVal );
	if( error != 0 )
	{
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "unknown key " + key + ": " + err );
	}

	if( enable )
		optVal = optVal |  flagOpt->default_val.i64;
	else
		optVal = optVal &~ flagOpt->default_val.i64;
	
	error = av_opt_set_int( _objContext, key.c_str(), optVal, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + flag + ": " + err );
	}
}

void ParamSet::set( const std::string& key, const bool value )
{
	int error = av_opt_set_int( _objContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + ( value ? "true" : "false" ) + ": " + err );
	}
}

void ParamSet::set( const std::string& key, const int value )
{
	//const AVOption* flagOpt = av_opt_find( _objContext, key.c_str(), NULL, 0, AV_OPT_SEARCH_CHILDREN );

	int error = av_opt_set_int( _objContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << value;
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + err );
	}
}

void ParamSet::set( const std::string& key, const int num, const int den )
{
	AVRational ratio;
	ratio.num = num;
	ratio.den = den;
	int error = av_opt_set_q( _objContext, key.c_str(), ratio, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << num << "/" << den;
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + err );
	}
}

void ParamSet::set( const std::string& key, const double value )
{
	int error = av_opt_set_double( _objContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << value;
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + err );
	}
}

void ParamSet::set( const std::string& key, const std::string& value )
{
	int error = av_opt_set( _objContext, key.c_str(), value.c_str(), AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::string err( "", AV_ERROR_MAX_STRING_SIZE );
		//av_make_error_string( const_cast<char*>(err.c_str()), err.size(), error );
		av_strerror( error, const_cast<char*>(err.c_str()), err.size() );
		throw std::runtime_error( "setting " + key + " parameter to " + value + ": " + err );
	}
}

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
