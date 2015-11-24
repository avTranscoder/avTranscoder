#ifndef _AV_TRANSCODER_OPTION_HPP
#define	_AV_TRANSCODER_OPTION_HPP

#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavutil/opt.h>
}

#include <vector>
#include <string>
#include <map>
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
class AvExport Option
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
	bool isEncodingOpt() const { return ( _avOption->flags & AV_OPT_FLAG_ENCODING_PARAM ) == AV_OPT_FLAG_ENCODING_PARAM; }
	bool isDecodingOpt() const { return ( _avOption->flags & AV_OPT_FLAG_DECODING_PARAM ) == AV_OPT_FLAG_DECODING_PARAM; }
	bool isAudioOpt() const { return ( _avOption->flags & AV_OPT_FLAG_AUDIO_PARAM ) == AV_OPT_FLAG_AUDIO_PARAM; }
	bool isVideoOpt() const { return ( _avOption->flags & AV_OPT_FLAG_VIDEO_PARAM ) == AV_OPT_FLAG_VIDEO_PARAM; }
	bool isSubtitleOpt() const { return ( _avOption->flags & AV_OPT_FLAG_SUBTITLE_PARAM ) == AV_OPT_FLAG_SUBTITLE_PARAM; }
	
	// get default value
	bool getDefaultBool() const;
	int getDefaultInt() const;
	double getDefaultDouble() const;
	std::string getDefaultString() const;
	std::pair<int, int> getDefaultRatio() const;

	// get value
	bool getBool() const;
	int getInt() const;
	double getDouble() const;
	std::string getString() const;
	std::pair<int, int> getRatio() const;

	// set value
	void setFlag( const std::string& flag, const bool enable );
	void setBool( const bool value );
	void setInt( const int value );
	void setDouble( const double value );
	void setString( const std::string& value );
	void setRatio( const int num, const int den );
	
	// array of childs
	bool hasChild() const { return _childOptions.size(); }
	const std::vector<Option>& getChilds() const { return _childOptions; }
	const Option& getChildAtIndex( const size_t index ) const { return _childOptions.at( index ); }
	int getDefaultChildIndex() const { return _defaultChildIndex; }
	
	void setDefaultChildIndex( size_t index ) { _defaultChildIndex = index; }
	void appendChild( const Option& child );
	
private:
	EOptionBaseType getTypeFromAVOption( const std::string& unit, const AVOptionType avType );

	/**
	 * @brief Check the return value from FFmpeg functions
	 * @note Throw run_time exception if error
	 */
	void checkFFmpegGetOption( const int ffmpegReturnCode ) const;
	void checkFFmpegSetOption( const int ffmpegReturnCode, const std::string& optionValue );

private:
	AVOption* _avOption;   ///< Has link (no ownership)
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

typedef std::vector<Option> OptionArray;
typedef std::map<std::string, Option> OptionMap;  ///< Key: option name / value: option

#ifndef SWIG
/**
 * @param outOptions: map or array of options
 * @param av_class: a libav context (could be an AVFormatContext or an AVCodecContext).
 * @param req_flags: libav flag (AV_OPT_FLAG_XXX), which is a filter for AVOption loaded by the Context (default = 0: no flag restriction).
 */
void AvExport loadOptions( OptionMap& outOptions, void* av_class, int req_flags = 0 );
void AvExport loadOptions( OptionArray& outOptions, void* av_class, int req_flags = 0 );
#endif

}

#endif
