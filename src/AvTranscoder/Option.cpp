#include "Option.hpp"

#include <stdexcept>

namespace avtranscoder
{

Option::Option( const AVOption& avOption )
	: m_name ( avOption.name )
	, m_help ( avOption.help ? avOption.help : "" )
	, m_flags( avOption.flags )
{
}

int Option::getDefaultValue( int& value ) const
{
	throw std::runtime_error( "Wrong access to int getDefaultValue" );
}

int64_t Option::getDefaultValue( int64_t& value ) const
{
	throw std::runtime_error( "Wrong access to int64_t getDefaultValue" );
}

double Option::getDefaultValue( double& value ) const
{
	throw std::runtime_error( "Wrong access to double getDefaultValue" );
}

std::string& Option::getDefaultValue( std::string& value )
{
	throw std::runtime_error( "Wrong access to char* getDefaultValue" );
}

}
