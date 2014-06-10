#ifndef _AV_TRANSCODER_OPTION_GROUP_HPP
#define	_AV_TRANSCODER_OPTION_GROUP_HPP

#include <AvTranscoder/Option.hpp>
#include <AvTranscoder/Options/OptionBoolean.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/opt.h>
}

namespace avtranscoder
{

/**
 * @brief Wrap of AVOption with a type of AV_OPT_TYPE_FLAGS and unit informed.
 */
class OptionGroup : public Option
{
public:
	OptionGroup( const AVOption& avOption );
	
	~OptionGroup();
	
	std::string getType() const { return "OptionGroup"; }
	int getDefaultValue( int& defaultValue ) const { return defaultValue = m_defaultValue; }
	
	std::vector< OptionBoolean >& getElements() { return m_elements; }
	OptionBoolean& getElement( size_t index ) { return m_elements.at( index ); }
	size_t getNbElements() const { return m_elements.size(); }
	
	void appendElement( const OptionBoolean& element );
	
private:
	int m_defaultValue;
	double m_minValue;
	double m_maxValue;
	
	std::vector< OptionBoolean > m_elements;
};

}

#endif
