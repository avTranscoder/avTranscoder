#ifndef _AV_TRANSCODER_OPTION_2D_HPP
#define	_AV_TRANSCODER_OPTION_2D_HPP

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
 * @brief Object return by getDefaultValue of Option2D.
 */
struct Value2D
{
	Value2D()
		: num( 0 )
		, dem( 0 )
	{}
	
	Value2D( int numValue, int demValue )
		: num( numValue )
		, dem( demValue )
	{}
	
	int num;
	int dem;
};

/**
 * @brief Wrap of AVOption with a type of AV_OPT_TYPE_RATIONAL.
 */
class Option2D : public Option
{
public:
	Option2D( const AVOption& avOption );
	
	~Option2D();
	
	AVOptionType getType() const { return AV_OPT_TYPE_RATIONAL; }
	Value2D& getDefaultValue( Value2D& defaultValue ) const { return defaultValue = m_defaultValue; }
	
private:
	Value2D m_defaultValue;
	double m_minValue;
	double m_maxValue;
};

}

#endif
