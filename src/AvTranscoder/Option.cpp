#include "Option.hpp"

namespace avtranscoder
{

Option::Option( const AVOption& avOption, OptionType type )
	: _avOption( avOption )
	, _type( type )
	, _options()
	, _defaultChildIndex( 0 )
{
	
}

OptionType Option::getTypeFromAVOption( const char* unit, AVOptionType avType )
{
	if( unit && avType == AV_OPT_TYPE_FLAGS )
		return TypeGroup;
	else if( unit && avType == AV_OPT_TYPE_INT )
		return TypeChoice;
	else if( unit && avType == AV_OPT_TYPE_CONST )
		return TypeChild;
	
	switch( avType )
	{
		case AV_OPT_TYPE_FLAGS:
		{
			return TypeBool;
		}
		case AV_OPT_TYPE_INT:
		case AV_OPT_TYPE_INT64:
		{
			return TypeInt;
		}
		case AV_OPT_TYPE_DOUBLE:
		case AV_OPT_TYPE_FLOAT:
		{
			return TypeDouble;
		}
		case AV_OPT_TYPE_STRING:
		case AV_OPT_TYPE_BINARY:
		{
			return TypeString;
		}
		case AV_OPT_TYPE_RATIONAL:
		{
			return TypeRatio;
		}
		default:
		{
			return TypeUnknown;
		}
	}
	return TypeUnknown;
}

OptionType Option::getType() const
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
