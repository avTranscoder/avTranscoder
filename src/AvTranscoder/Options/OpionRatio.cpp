#include "OpionRatio.hpp"


namespace avtranscoder
{

OpionRatio::OpionRatio( const AVOption& avOption )
	: Option( avOption )
	, m_defaultValue( avOption.default_val.q.num, avOption.default_val.q.den )
	, m_minValue( avOption.min )
	, m_maxValue( avOption.max )
{
	
}

OpionRatio::~OpionRatio()
{
	
}

}
