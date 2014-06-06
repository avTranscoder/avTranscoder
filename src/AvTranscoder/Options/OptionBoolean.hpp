#ifndef _AV_TRANSCODER_OPTION_BOOLEAN_HPP
#define	_AV_TRANSCODER_OPTION_BOOLEAN_HPP

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
 * @brief Wrap of AVOption with a type of AV_OPT_TYPE_FLAGS and no unit informed..
 */
class OptionBoolean : public Option
{
public:
	OptionBoolean( const AVOption& avOption );
	
	~OptionBoolean();
	
	AVOptionType getType() const { return AV_OPT_TYPE_FLAGS; }
	bool getDefaultValue( bool& defaultValue ) const { return defaultValue = m_defaultValue; }
	
private:
	bool m_defaultValue;
	double m_minValue;
	double m_maxValue;
};

}

#endif
