#include "Context.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/opt.h>
}

#include <sstream>
#include <stdexcept>

namespace avtranscoder
{

void Context::set( const std::string& key, const std::string& flag, const bool enable )
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
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "unknown key " + key + ": " + err );
	}

	if( enable )
		optVal = optVal |  flagOpt->default_val.i64;
	else
		optVal = optVal &~ flagOpt->default_val.i64;
	
	error = av_opt_set_int( _objContext, key.c_str(), optVal, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + key + " parameter to " + flag + ": " + err );
	}
}

void Context::set( const std::string& key, const bool value )
{
	int error = av_opt_set_int( _objContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + key + " parameter to " + ( value ? "true" : "false" ) + ": " + err );
	}
}

void Context::set( const std::string& key, const int value )
{
	//const AVOption* flagOpt = av_opt_find( _objContext, key.c_str(), NULL, 0, AV_OPT_SEARCH_CHILDREN );

	int error = av_opt_set_int( _objContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << value;
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + err );
	}
}

void Context::set( const std::string& key, const int num, const int den )
{
	AVRational ratio;
	ratio.num = num;
	ratio.den = den;
	int error = av_opt_set_q( _objContext, key.c_str(), ratio, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << num << "/" << den;
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + err );
	}
}

void Context::set( const std::string& key, const double value )
{
	int error = av_opt_set_double( _objContext, key.c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		std::ostringstream os;
		os << value;
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + key + " parameter to " + os.str() + ": " + err );
	}
}

void Context::set( const std::string& key, const std::string& value )
{
	int error = av_opt_set( _objContext, key.c_str(), value.c_str(), AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + key + " parameter to " + value + ": " + err );
	}
}

}
