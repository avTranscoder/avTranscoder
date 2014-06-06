#include "OptionChoice.hpp"


namespace avtranscoder
{

OptionChoice::OptionChoice( const AVOption& avOption )
	: Option( avOption )
	, m_defaultValue( avOption.default_val.dbl )
	, m_minValue( avOption.min )
	, m_maxValue( avOption.max )
	, m_choices()
	, m_defaultChoiceIndex( 0 )
{
	
}

OptionChoice::~OptionChoice()
{
	
}

void OptionChoice::appendOption( const char* name, const char* help )
{
	m_choices.push_back( std::pair<std::string, std::string>( name, help ) );
}

void OptionChoice::appendOption( const char* name )
{
	m_choices.push_back( std::pair<std::string, std::string>( name, "" ) );
}

}
