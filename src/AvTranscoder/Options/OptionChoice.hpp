#ifndef _AV_TRANSCODER_OPTION_CHOICE_HPP
#define	_AV_TRANSCODER_OPTION_CHOICE_HPP

#include <AvTranscoder/Option.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/opt.h>
}

#include <vector>
#include <utility> //pair

namespace avtranscoder
{

/**
 * @brief Wrap of AVOption with a type of AV_OPT_TYPE_INT and unit informed.
 */
class OptionChoice : public Option
{
public:
	OptionChoice( const AVOption& avOption );
	
	~OptionChoice();
	
	std::string getType() const { return "OptionChoice"; }
	double getDefaultValue( double& defaultValue ) const { return defaultValue = m_defaultValue; }
	
	std::vector< std::pair<std::string, std::string> >& getChoices() { return m_choices; }
	std::pair<std::string, std::string>& getChoice( size_t index ) { return m_choices.at( index ); }
	size_t getNbChoices() const { return m_choices.size(); }
	int getDefaultChoiceIndex( ) const { return m_defaultChoiceIndex; }
	
	void setDefaultChoiceIndex( int defaultChoiceIndex ) { m_defaultChoiceIndex = defaultChoiceIndex; }
	
	void appendChoice( const char* name, const char* help );
	void appendChoice( const char* name );

private:
	double m_defaultValue;
	double m_minValue;
	double m_maxValue;
	
	std::vector< std::pair<std::string, std::string> > m_choices;
	int m_defaultChoiceIndex;
};

}

#endif
