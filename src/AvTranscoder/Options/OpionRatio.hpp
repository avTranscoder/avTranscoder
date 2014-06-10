#ifndef _AV_TRANSCODER_OPTION_2D_HPP
#define _AV_TRANSCODER_OPTION_2D_HPP

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
 * @brief Wrap of AVOption with a type of AV_OPT_TYPE_RATIONAL.
 */
class OpionRatio : public Option
{
public:
	OpionRatio( const AVOption& avOption );
	
	~OpionRatio();
	
	std::string getType() const { return "OptionRatio"; }
	std::pair<int, int>& getDefaultValue( std::pair<int, int>& defaultValue ) const { return defaultValue = m_defaultValue; }
	
private:
	std::pair<int, int> m_defaultValue;
	double m_minValue;
	double m_maxValue;
};

}

#endif
