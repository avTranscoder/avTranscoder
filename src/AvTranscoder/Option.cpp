#include "Option.hpp"

#include <stdexcept>

namespace avtranscoder
{

Option::Option( const AVOption& avOption )
	: m_name ( avOption.name )
	, m_help ( avOption.help ? avOption.help : "" )
	, m_unit ( avOption.unit ? avOption.unit : "" )
	, m_flags( avOption.flags )
{
}

int Option::getDefaultValue( int& value ) const
{
	throw std::runtime_error( "Wrong access to getDefaultValue (int)." );
}

double Option::getDefaultValue( double& value ) const
{
	throw std::runtime_error( "Wrong access to getDefaultValue (double)." );
}

std::string& Option::getDefaultValue( std::string& value ) const
{
	throw std::runtime_error( "Wrong access to getDefaultValue (char*)." );
}

bool Option::getDefaultValue( bool& value ) const
{
	throw std::runtime_error( "Wrong access to getDefaultValue (bool)." );
}

std::pair<int, int>& Option::getDefaultValue( std::pair<int, int>& value ) const
{
	throw std::runtime_error( "Wrong access to getDefaultValue (std::pair<int, int>)." );
}

}
