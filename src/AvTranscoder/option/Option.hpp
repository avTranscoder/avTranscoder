#ifndef _AV_TRANSCODER_OPTION_HPP
#define	_AV_TRANSCODER_OPTION_HPP

#include <AvTranscoder/common.hpp>

extern "C" {
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
	Option( AVOption& avOption, void* avContext );
	~Option() {}
	
	EOptionBaseType getType() const;

	std::string getName() const { return std::string( _avOption->name ? _avOption->name : "" ); }
	std::string getHelp() const { return std::string( _avOption->help ? _avOption->help : "" ); }
	std::string getUnit() const { return std::string( _avOption->unit ? _avOption->unit : "" ); }
	int getOffset() const { return _avOption->offset; }
	double getMin() const { return _avOption->min; }
	double getMax() const { return _avOption->max; }
	
	// flags
	int getFlags() const { return _avOption->flags; }
	bool isEncodingOpt() const { return _avOption->flags & AV_OPT_FLAG_ENCODING_PARAM; }
	bool isDecodingOpt() const { return _avOption->flags & AV_OPT_FLAG_DECODING_PARAM; }
	bool isAudioOpt() const { return _avOption->flags & AV_OPT_FLAG_AUDIO_PARAM; }
	bool isVideoOpt() const { return _avOption->flags & AV_OPT_FLAG_VIDEO_PARAM; }
	bool isSubtitleOpt() const { return _avOption->flags & AV_OPT_FLAG_SUBTITLE_PARAM; }
	
	// get default value
	bool getDefaultBool() const;
	int getDefaultInt() const;
	double getDefaultDouble() const;
	std::string getDefaultString() const;
	std::pair<int, int> getDefaultRatio() const;

	// set value
	void setFlag( const std::string& flag, const bool enable );
	void setBool( const bool value );
	void setInt( const int value );
	void setRatio( const int num, const int den );
	void setDouble( const double value );
	void setString( const std::string& value );
	
	// array of childs
	bool hasChild() const { return _childOptions.size(); }
	const std::vector<Option>& getChilds() const { return _childOptions; }
	const Option& getChildAtIndex( const size_t index ) const { return _childOptions.at( index ); }
	int getDefaultChildIndex() const { return _defaultChildIndex; }
	
	void setDefaultChildIndex( size_t index ) { _defaultChildIndex = index; }
	void appendChild( const Option& child );
	
private:
	EOptionBaseType getTypeFromAVOption( const std::string& unit, const AVOptionType avType );

private:
	AVOption* _avOption;
	EOptionBaseType _type;

	/**
	 * @brief Pointer to the corresponding context.
	 * Need it to set and get the option values.
	 */
	void* _avContext; ///< Has link (no ownership)
	
	/**
	 * If the option corresponds to a Choice or a Group, it can contain childs,
	 * which are also options.
	 */
	std::vector<Option> _childOptions;
	size_t _defaultChildIndex;
};

}

#endif
