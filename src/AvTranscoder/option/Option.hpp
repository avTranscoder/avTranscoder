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
#include <utility>

namespace avtranscoder
{

enum EOptionBaseType
{
	eOptionBaseTypeBool,
	eOptionBaseTypeInt,
	eOptionBaseTypeDouble,
	eOptionBaseTypeString,
	eOptionBaseTypeRatio,
	eOptionBaseTypeChoice,
	eOptionBaseTypeGroup,
	eOptionBaseTypeChild, // Option which belongs to Choice or Group
	eOptionBaseTypeUnknown
};

/**
 * @brief Wrapper of AVOption.
 * Get its type to know what the option is about: Int, Double, Ratio, Choice...
 * Parse its array of options to get the potential childs (Choice and Group).
 */
class Option
{
public:
	Option( const AVOption& avOption );
	~Option() {}
	
	EOptionBaseType getType() const;

	std::string getName() const { return std::string( _avOption.name ? _avOption.name : "" ); }
	std::string getHelp() const { return std::string( _avOption.help ? _avOption.help : "" ); }
	std::string getUnit() const { return std::string( _avOption.unit ? _avOption.unit : "" ); }
	int getOffset() const { return _avOption.offset; }
	double getMin() const { return _avOption.min; }
	double getMax() const { return _avOption.max; }
	
	// flags
	int getFlags() const { return _avOption.flags; }
	bool isEncodingOpt() const { return _avOption.flags & AV_OPT_FLAG_ENCODING_PARAM; }
	bool isDecodingOpt() const { return _avOption.flags & AV_OPT_FLAG_DECODING_PARAM; }
	bool isAudioOpt() const { return _avOption.flags & AV_OPT_FLAG_AUDIO_PARAM; }
	bool isVideoOpt() const { return _avOption.flags & AV_OPT_FLAG_VIDEO_PARAM; }
	bool isSubtitleOpt() const { return _avOption.flags & AV_OPT_FLAG_SUBTITLE_PARAM; }
	
	// default value
	bool getDefaultValueBool() const;
	int getDefaultValueInt() const;
	double getDefaultValueDouble() const;
	std::string getDefaultValueString() const;
	std::pair<int, int> getDefaultValueRatio() const;
	
	// array of childs
	bool hasChild() const { return ! _options.empty(); }
	const std::vector<Option>& getChilds() { return _options; }
	const Option& getChild( size_t index ) { return _options.at( index ); }
	size_t getNbChilds() const { return _options.size(); }
	int getDefaultChildIndex() const { return _defaultChildIndex; }
	
	void setDefaultChildIndex( size_t index ) { _defaultChildIndex = index; }
	void appendChild( const Option& child );
	
private:
	EOptionBaseType getTypeFromAVOption( const std::string& unit, const AVOptionType avType );

private:
	AVOption _avOption;
	EOptionBaseType _type;
	
	/**
	 * If the option corresponds to a Choice or a Group, it can contain childs,
	 * which are also options.
	 */
	std::vector<Option> _options;
	size_t _defaultChildIndex;
};

}

#endif
