#include "OptionBoolean.hpp"


namespace avtranscoder
{

OptionBoolean::OptionBoolean( const AVOption& avOption )
	: Option( avOption )
	, m_defaultValue( (bool)avOption.default_val.i64 )
	, m_minValue( avOption.min )
	, m_maxValue( avOption.max )
{
	
}

OptionBoolean::~OptionBoolean()
{
	
}

}
