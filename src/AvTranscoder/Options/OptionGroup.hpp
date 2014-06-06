#ifndef _AV_TRANSCODER_OPTION_GROUP_HPP
#define	_AV_TRANSCODER_OPTION_GROUP_HPP

#include <AvTranscoder/Option.hpp>

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
	
	AVOptionType getType() const { return AV_OPT_TYPE_FLAGS; }
	int getDefaultValue( int& defaultValue ) const { return defaultValue = m_defaultValue; }
	
private:
	int m_defaultValue;
	double m_minValue;
	double m_maxValue;
};

}

#endif
