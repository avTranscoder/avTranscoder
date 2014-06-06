#include "OptionString.hpp"


namespace avtranscoder
{

OptionString::OptionString( const AVOption& avOption )
	: Option( avOption )
	, m_defaultValue( avOption.default_val.str ? avOption.default_val.str : "" )
	, m_minValue( avOption.min )
	, m_maxValue( avOption.max )
{
	
}

OptionString::~OptionString()
{
	
}

}
