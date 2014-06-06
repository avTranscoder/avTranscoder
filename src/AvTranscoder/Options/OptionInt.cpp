#include "OptionInt.hpp"


namespace avtranscoder
{

OptionInt::OptionInt( const AVOption& avOption )
	: Option( avOption )
	, m_defaultValue( avOption.default_val.i64 )
	, m_minValue( avOption.min )
	, m_maxValue( avOption.max )
{
	
}

OptionInt::~OptionInt()
{
	
}

}
