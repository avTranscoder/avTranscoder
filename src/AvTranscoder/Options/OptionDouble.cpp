#include "OptionDouble.hpp"


namespace avtranscoder
{

OptionDouble::OptionDouble( const AVOption& avOption )
	: Option( avOption )
	, m_defaultValue( avOption.default_val.dbl )
	, m_minValue( avOption.min )
	, m_maxValue( avOption.max )
{
	
}

OptionDouble::~OptionDouble()
{
	
}

}
