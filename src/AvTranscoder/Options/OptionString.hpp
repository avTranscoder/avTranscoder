#ifndef _AV_TRANSCODER_OPTION_STRING_HPP
#define	_AV_TRANSCODER_OPTION_STRING_HPP

#include <AvTranscoder/Option.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/opt.h>
}

#include <string>

namespace avtranscoder
{

class OptionString : public Option
{
public:
	OptionString( const AVOption& avOption );
	
	~OptionString();
	
	AVOptionType getType() const { return AV_OPT_TYPE_STRING; }
	std::string& getDefaultValue( std::string& defaultValue ) const { return defaultValue = m_defaultValue; }
	
private:
	std::string m_defaultValue;
	double m_minValue;
	double m_maxValue;
};

}

#endif
