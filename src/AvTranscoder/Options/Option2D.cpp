#include "Option2D.hpp"


namespace avtranscoder
{

Option2D::Option2D( const AVOption& avOption )
	: Option( avOption )
	, m_defaultValue( avOption.default_val.q.num, avOption.default_val.q.den )
	, m_minValue( avOption.min )
	, m_maxValue( avOption.max )
{
	
}

Option2D::~Option2D()
{
	
}

}
