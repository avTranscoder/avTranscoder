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
#include <string>
#include <utility> //pair

namespace avtranscoder
{

/**
 * @brief Abstract class to wrap AVOption.
 * Subclasses  implement specific AVOption: int, boolean...
 */
class Option
{	
public:
	Option( const AVOption& avOption );
	virtual ~Option() {}
	
	virtual std::string getType() const = 0;
	
	virtual int getDefaultValue( int& value ) const;
	virtual double getDefaultValue( double& value ) const;
	virtual std::string& getDefaultValue( std::string& value ) const;
	virtual bool getDefaultValue( bool& value ) const;
	virtual std::pair<int, int>& getDefaultValue( std::pair<int, int>& value ) const;
	
	std::string& getName() { return m_name; }
	std::string& getHelp() { return m_help; }
	std::string& getUnit() { return m_unit; }
	
	bool isEncodingOpt(){ return m_flags & AV_OPT_FLAG_ENCODING_PARAM; }
	bool isDecodingOpt(){ return m_flags & AV_OPT_FLAG_DECODING_PARAM; }
	bool isAudioOpt(){ return m_flags & AV_OPT_FLAG_AUDIO_PARAM; }
	bool isVideoOpt(){ return m_flags & AV_OPT_FLAG_VIDEO_PARAM; }
	bool isSubtitleOpt(){ return m_flags & AV_OPT_FLAG_SUBTITLE_PARAM; }
	
protected:
	std::string m_name;
	std::string m_help;
	std::string m_unit;
	int         m_flags;
};

}

#endif
