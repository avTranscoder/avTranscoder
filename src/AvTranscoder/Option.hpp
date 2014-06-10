#ifndef _AV_TRANSCODER_OPTION_HPP
#define	_AV_TRANSCODER_OPTION_HPP

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/opt.h>
}

#include <vector>
#include <string>
#include <utility> //pair

namespace avtranscoder
{

enum OptionType {
	TypeBool,
	TypeInt,
	TypeDouble,
	TypeString,
	TypeRatio,
	TypeChoice,
	TypeGroup,
	
	TypeChild, // Option which brelongs to Choice or Group
	TypeUnknown
};

/**
 * @brief Wrapper of AVOption.
 * Get its type to know what the option is about: Int, Double, Ratio, Choice...
 * Parse its array of options to get the potential childs (Choice and Group).
 */
class Option
{
public:
	static OptionType getTypeFromAVOption( const char* unit, AVOptionType avType );
	
public:
	Option( const AVOption& avOption, OptionType type );
	~Option() {}
	
	OptionType getType() const;
	const std::string getName() { return std::string( m_avOption.name ? m_avOption.name : "" ); }
	const std::string getHelp() { return std::string( m_avOption.help ? m_avOption.help : "" ); }
	const std::string getUnit() { return std::string( m_avOption.unit ? m_avOption.unit : "" ); }
	
	// flags
	int getFlags() { return m_avOption.flags; }
	bool isEncodingOpt(){ return m_avOption.flags & AV_OPT_FLAG_ENCODING_PARAM; }
	bool isDecodingOpt(){ return m_avOption.flags & AV_OPT_FLAG_DECODING_PARAM; }
	bool isAudioOpt(){ return m_avOption.flags & AV_OPT_FLAG_AUDIO_PARAM; }
	bool isVideoOpt(){ return m_avOption.flags & AV_OPT_FLAG_VIDEO_PARAM; }
	bool isSubtitleOpt(){ return m_avOption.flags & AV_OPT_FLAG_SUBTITLE_PARAM; }
	
	// default value
	bool getDefaultValueBool() const;
	int getDefaultValueInt() const;
	double getDefaultValueDouble() const;
	std::string getDefaultValueString() const;
	std::pair<int, int> getDefaultValueRatio() const;
	
	// array of childs
	bool hasChild() { return ! m_options.empty(); }
	std::vector<Option> getChilds() { return m_options; }
	Option& getChild( size_t index ) { return m_options.at( index ); }
	size_t getNbChilds() { return m_options.size(); }
	int getDefaultChildIndex() const { return m_defaultChildIndex; }
	void setDefaultChildIndex( size_t index ) { m_defaultChildIndex = index; }
	void appendChild( const Option& child );
	
private:
	AVOption m_avOption;
	OptionType m_type;
	
	/**
	 * If the option corresponds to a Choice or a Group, it can contain childs,
	 * which are also options.
	 */
	std::vector<Option> m_options;
	size_t m_defaultChildIndex;
};

}

#endif
