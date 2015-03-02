#include "Option.hpp"

extern "C" {
#include <libavutil/error.h>
#include <libavutil/mem.h>
}

#include <iostream>
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

bool Option::getDefaultBool() const
{
	return _avOption->default_val.i64;
}

int Option::getDefaultInt() const
{
	return _avOption->default_val.i64;
}

double Option::getDefaultDouble() const
{
	return _avOption->default_val.dbl;
}

std::string Option::getDefaultString() const
{
	return std::string( _avOption->default_val.str ? _avOption->default_val.str : "" );
}

std::pair<int, int> Option::getDefaultRatio() const
{
	return std::make_pair( _avOption->default_val.q.num, _avOption->default_val.q.den );
}

bool Option::getBool() const
{
	int64_t out_val;
	int error = av_opt_get_int( _avContext, getName().c_str(), AV_OPT_SEARCH_CHILDREN, &out_val );
	checkFFmpegGetOption( error );

	return out_val ? true : false;
}

int Option::getInt() const
{
	int64_t out_val;
	int error =  av_opt_get_int( _avContext, getName().c_str(), AV_OPT_SEARCH_CHILDREN, &out_val );
	checkFFmpegGetOption( error );

	return out_val;
}

double Option::getDouble() const
{
	double out_val;
	int error = av_opt_get_double( _avContext, getName().c_str(), AV_OPT_SEARCH_CHILDREN, &out_val );
	checkFFmpegGetOption( error );

	return out_val;
}

std::string Option::getString() const
{
	void* out_val = av_malloc( 128 );
	int error = av_opt_get( _avContext, getName().c_str(), AV_OPT_SEARCH_CHILDREN, (uint8_t**)&out_val );

	std::string strValue( out_val ? reinterpret_cast<const char*>( out_val ) : "" );

	av_free( out_val );

	checkFFmpegGetOption( error );

	return strValue;
}

std::pair<int, int> Option::getRatio() const
{
	Rational out_val;
	int error = av_opt_get_q( _avContext, getName().c_str(), AV_OPT_SEARCH_CHILDREN, &out_val );
	checkFFmpegGetOption( error );

	return std::make_pair( out_val.num, out_val.den );
}

void Option::setFlag( const std::string& flag, const bool enable )
{
	int64_t optVal;
	int error = av_opt_get_int( _avContext, getName().c_str(), AV_OPT_SEARCH_CHILDREN, &optVal );
	checkFFmpegGetOption( error );

	if( enable )
		optVal = optVal |  _avOption->default_val.i64;
	else
		optVal = optVal &~ _avOption->default_val.i64;

	error = av_opt_set_int( _avContext, getName().c_str(), optVal, AV_OPT_SEARCH_CHILDREN );
	checkFFmpegSetOption( error, flag );
}

void Option::setBool( const bool value )
{
	int error = av_opt_set_int( _avContext, getName().c_str(), value, AV_OPT_SEARCH_CHILDREN );

	std::ostringstream os;
	os << ( value ? "true" : "false" );
	checkFFmpegSetOption( error, os.str() );
}

void Option::setInt( const int value )
{
	int error = av_opt_set_int( _avContext, getName().c_str(), value, AV_OPT_SEARCH_CHILDREN );

	std::ostringstream os;
	os << value;
	checkFFmpegSetOption( error, os.str() );
}

void Option::setDouble( const double value )
{
	int error = av_opt_set_double( _avContext, getName().c_str(), value, AV_OPT_SEARCH_CHILDREN );

	std::ostringstream os;
	os << value;
	checkFFmpegSetOption( error, os.str() );
}

void Option::setString( const std::string& value )
{
	int error = av_opt_set( _avContext, getName().c_str(), value.c_str(), AV_OPT_SEARCH_CHILDREN );
	checkFFmpegSetOption( error, value );
}

void Option::setRatio( const int num, const int den )
{
	Rational ratio;
	ratio.num = num;
	ratio.den = den;
	int error = av_opt_set_q( _avContext, getName().c_str(), ratio, AV_OPT_SEARCH_CHILDREN );

	std::ostringstream os;
	os << num << "/" << den;
	checkFFmpegSetOption( error, os.str() );
}

void Option::appendChild( const Option& child )
{
	_childOptions.push_back( child );
}

void Option::checkFFmpegGetOption( const int ffmpegReturnCode ) const
{
	if( ffmpegReturnCode )
	{
		throw std::runtime_error( "unknown key " + getName() + ": " + getDescriptionFromErrorCode( ffmpegReturnCode ) );
	}
}

void Option::checkFFmpegSetOption( const int ffmpegReturnCode, const std::string& optionValue )
{
	if( ffmpegReturnCode )
	{
		throw std::runtime_error( "setting " + getName() + " parameter to " + optionValue + ": " + getDescriptionFromErrorCode( ffmpegReturnCode ) );
	}
}

void loadOptions( OptionMap& outOptions, void* av_class, int req_flags )
{
	if( ! av_class )
		return;

	std::multimap<std::string, std::string> optionUnitToParentName;
	std::vector<Option> childOptions;
	
	const AVOption* avOption = NULL;
	
	// iterate on options
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 51, 12, 0 )
	while( ( avOption = av_next_option( av_class, avOption ) ) )
#else
	while( ( avOption = av_opt_next( av_class, avOption ) ) )
#endif
	{
		if( ! avOption || 
			! avOption->name ||
			( avOption->flags & req_flags ) != req_flags )
		{
			continue;
		}

		Option option( *const_cast<AVOption*>( avOption ), av_class );

		if( option.getType() == eOptionBaseTypeChild )
		{
			childOptions.push_back( option );
		}
		else
		{
			outOptions.insert( std::make_pair( option.getName(), option ) );
			optionUnitToParentName.insert( std::make_pair( option.getUnit(), option.getName() ) );
		}
	}

	// iterate on child options
	for( std::vector<Option>::iterator itOption = childOptions.begin(); itOption != childOptions.end(); ++itOption )
	{
		bool parentFound = false;
		for( std::multimap<std::string, std::string>::iterator itUnit = optionUnitToParentName.begin(); itUnit != optionUnitToParentName.end(); ++itUnit )
		{
			if( itUnit->first == itOption->getUnit() )
			{
				std::string nameParentOption = itUnit->second;
				Option& parentOption = outOptions.at( nameParentOption );

				parentOption.appendChild( *itOption );

				// child of a Choice
				if( parentOption.getType() == eOptionBaseTypeChoice )
				{
					if( itOption->getDefaultInt() == parentOption.getDefaultInt() )
						parentOption.setDefaultChildIndex( parentOption.getChilds().size() - 1 );
				}

				parentFound = true;
				break;
			}
		}

		if( ! parentFound )
		{
			std::cout << "Warning: Can't find a choice option for " << itOption->getName() << std::endl;
		}
	}
}

void loadOptions( OptionArray& outOptions, void* av_class, int req_flags )
{
	OptionMap optionMap;
	loadOptions( optionMap, av_class, req_flags );

	for( OptionMap::iterator it = optionMap.begin(); it != optionMap.end(); ++it )
		outOptions.push_back( it->second );
}

}
