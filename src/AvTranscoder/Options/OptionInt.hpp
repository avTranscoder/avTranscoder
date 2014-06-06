#ifndef _AV_TRANSCODER_OPTION_INT_HPP
#define	_AV_TRANSCODER_OPTION_INT_HPP

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
 * @brief Wrap of AVOption with a type of AV_OPT_TYPE_INT.
 */
class OptionInt : public Option
{
public:
	OptionInt( const AVOption& avOption );
	
	~OptionInt();
	
	AVOptionType getType() const { return AV_OPT_TYPE_INT; }
	int getDefaultValue( int& defaultValue ) const { return defaultValue = m_defaultValue; }
	
private:
	int m_defaultValue;
	double m_minValue;
	double m_maxValue;
};

}

#endif
