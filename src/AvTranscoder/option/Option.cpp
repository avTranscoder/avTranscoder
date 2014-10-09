#include "Option.hpp"

extern "C" {
#include <libavutil/error.h>
}

#include <sstream>
#include <stdexcept>

namespace avtranscoder
{

Option::Option( AVOption& avOption, void* avContext )
	: _avOption( &avOption )
	, _avContext( avContext )
	, _childOptions()
	, _defaultChildIndex( 0 )
{
	_type = getTypeFromAVOption( getUnit(), _avOption->type );
}

EOptionBaseType Option::getTypeFromAVOption( const std::string& unit, const AVOptionType avType )
{
	if( ! unit.empty() && avType == AV_OPT_TYPE_FLAGS )
		return eOptionBaseTypeGroup;
	else if( ! unit.empty() && ( avType == AV_OPT_TYPE_INT || avType == AV_OPT_TYPE_INT64 ) )
		return eOptionBaseTypeChoice;
	else if( ! unit.empty() && avType == AV_OPT_TYPE_CONST )
		return eOptionBaseTypeChild;
	
	switch( avType )
	{
		case AV_OPT_TYPE_FLAGS:
		{
			return eOptionBaseTypeBool;
		}
		case AV_OPT_TYPE_INT:
		case AV_OPT_TYPE_INT64:
		{
			return eOptionBaseTypeInt;
		}
		case AV_OPT_TYPE_DOUBLE:
		case AV_OPT_TYPE_FLOAT:
		{
			return eOptionBaseTypeDouble;
		}
		case AV_OPT_TYPE_STRING:
		case AV_OPT_TYPE_BINARY:
		{
			return eOptionBaseTypeString;
		}
		case AV_OPT_TYPE_RATIONAL:
		{
			return eOptionBaseTypeRatio;
		}
		default:
		{
			return eOptionBaseTypeUnknown;
		}
	}
	return eOptionBaseTypeUnknown;
}

EOptionBaseType Option::getType() const
{
	return _type;
}

bool Option::getDefaultValueBool() const
{
	return _avOption->default_val.i64;
}

int Option::getDefaultValueInt() const
{
	return _avOption->default_val.i64;
}

double Option::getDefaultValueDouble() const
{
	return _avOption->default_val.dbl;
}

std::string Option::getDefaultValueString() const
{
	return std::string( _avOption->default_val.str ? _avOption->default_val.str : "" );
}

std::pair<int, int> Option::getDefaultValueRatio() const
{
	return std::pair<int, int>( _avOption->default_val.q.num, _avOption->default_val.q.den );
}
//av_make_error_string
void Option::setFlag( const std::string& flag, const bool enable )
{
	int64_t optVal;
	int error = av_opt_get_int( _avContext, getName().c_str(), AV_OPT_SEARCH_CHILDREN, &optVal );
	if( ! error )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "unknown key " + getName() + ": " + err );
	}

	if( enable )
		optVal = optVal |  _avOption->default_val.i64;
	else
		optVal = optVal &~ _avOption->default_val.i64;

	error = av_opt_set_int( _avContext, getName().c_str(), optVal, AV_OPT_SEARCH_CHILDREN );
	if( error != 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + getName() + " parameter to " + flag + ": " + err );
	}
}

void Option::setValueBool( const bool value )
{
	int error = av_opt_set_int( _avContext, getName().c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( ! error )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + getName() + " parameter to " + ( value ? "true" : "false" ) + ": " + err );
	}
}

void Option::setValueInt( const int value )
{
	int error = av_opt_set_int( _avContext, getName().c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( ! error )
	{
		std::ostringstream os;
		os << value;
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + getName() + " parameter to " + os.str() + ": " + err );
	}
}

void Option::setValueRatio( const int num, const int den )
{
	AVRational ratio;
	ratio.num = num;
	ratio.den = den;
	int error = av_opt_set_q( _avContext, getName().c_str(), ratio, AV_OPT_SEARCH_CHILDREN );
	if( ! error )
	{
		std::ostringstream os;
		os << num << "/" << den;
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + getName() + " parameter to " + os.str() + ": " + err );
	}
}

void Option::setValueDouble( const double value )
{
	int error = av_opt_set_double( _avContext, getName().c_str(), value, AV_OPT_SEARCH_CHILDREN );
	if( ! error )
	{
		std::ostringstream os;
		os << value;
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + getName() + " parameter to " + os.str() + ": " + err );
	}
}

void Option::setValueString( const std::string& value )
{
	int error = av_opt_set( _avContext, getName().c_str(), value.c_str(), AV_OPT_SEARCH_CHILDREN );
	if( ! error )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( error, err, AV_ERROR_MAX_STRING_SIZE );
		throw std::runtime_error( "setting " + getName() + " parameter to " + value + ": " + err );
	}
}

void Option::appendChild( const Option& child )
{
	_childOptions.push_back( child );
}

}
