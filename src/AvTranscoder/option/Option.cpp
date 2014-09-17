#include "Option.hpp"

namespace avtranscoder
{

Option::Option( const AVOption& avOption )
	: _avOption( avOption )
	, _options()
	, _defaultChildIndex( 0 )
{
	_type = getTypeFromAVOption( getUnit(), _avOption.type );
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
	return _avOption.default_val.i64;
}

int Option::getDefaultValueInt() const
{
	return _avOption.default_val.i64;
}

double Option::getDefaultValueDouble() const
{
	return _avOption.default_val.dbl;
}

std::string Option::getDefaultValueString() const
{
	return std::string( _avOption.default_val.str ? _avOption.default_val.str : "" );
}

std::pair<int, int> Option::getDefaultValueRatio() const
{
	return std::pair<int, int>( _avOption.default_val.q.num, _avOption.default_val.q.den );
}

void Option::appendChild( const Option& child )
{
	_options.push_back( child );
}

}
