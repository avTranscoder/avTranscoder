#ifndef _AV_TRANSCODER_OPTION_HPP
#define	_AV_TRANSCODER_OPTION_HPP

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/opt.h>
}

#include <vector>
#include <bitset>

namespace avtranscoder
{

class Value2D;

/**
 * @breif Abstract class to wrap AVOption.
 * Subclasses  implement specific AVOption: int, boolean...
 */
class Option
{	
public:
	Option( const AVOption& avOption );
	virtual ~Option() {}
	
	virtual AVOptionType getType() const = 0;
	
	virtual int getDefaultValue( int& value ) const;
	virtual double getDefaultValue( double& value ) const;
	virtual std::string& getDefaultValue( std::string& value ) const;
	virtual bool getDefaultValue( bool& value ) const;
	virtual Value2D& getDefaultValue( Value2D& value ) const;
	
	std::string& getName() { return m_name; }
	std::string& getHelp() { return m_help; }
	
	bool isEncodingOpt(){ return m_flags & AV_OPT_FLAG_ENCODING_PARAM; }
	bool isDecodingOpt(){ return m_flags & AV_OPT_FLAG_DECODING_PARAM; }
	bool isAudioOpt(){ return m_flags & AV_OPT_FLAG_AUDIO_PARAM; }
	bool isVideoOpt(){ return m_flags & AV_OPT_FLAG_VIDEO_PARAM; }
	bool isSubtitleOpt(){ return m_flags & AV_OPT_FLAG_SUBTITLE_PARAM; }
	
protected:
	std::string m_name;
	std::string m_help;
	int         m_flags;
};

}

#endif
