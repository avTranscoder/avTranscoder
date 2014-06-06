#ifndef _AV_TRANSCODER_OPTION_DOUBLE_HPP
#define	_AV_TRANSCODER_OPTION_DOUBLE_HPP

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
 * @brief Wrap of AVOption with a type of AV_OPT_TYPE_DOUBLE.
 */
class OptionDouble : public Option
{
public:
	OptionDouble( const AVOption& avOption );
	
	~OptionDouble();
	
	AVOptionType getType() const { return AV_OPT_TYPE_DOUBLE; }
	double getDefaultValue( double &defaultValue ) const { return defaultValue = m_defaultValue; }

private:
	double m_defaultValue;
	double m_minValue;
	double m_maxValue;

};

}

#endif