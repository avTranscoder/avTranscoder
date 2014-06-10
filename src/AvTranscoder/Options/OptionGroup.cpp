#include "OptionGroup.hpp"


namespace avtranscoder
{

OptionGroup::OptionGroup( const AVOption& avOption )
	: Option( avOption )
	, m_defaultValue( avOption.default_val.i64 )
	, m_elements()
{
	std::string name = "g_";
	name += avOption.unit;
	m_name = name;
}

OptionGroup::~OptionGroup()
{
	
}

void OptionGroup::appendElement( const OptionBoolean& element )
{
	m_elements.push_back( element );
}

}
