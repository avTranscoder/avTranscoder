#ifndef _AV_TRANSCODER_CONTEXT_HPP_
#define _AV_TRANSCODER_CONTEXT_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/option/Option.hpp>

#include <string>
#include <map>

namespace avtranscoder
{

typedef std::vector<Option> OptionArray;
typedef std::map<std::string, Option> OptionMap;

/**
 * @brief Wrapper of AVContext.
 * Can access Options through the corresponding context.
 */
class AvExport Context
{
public:
	Context() {}

	/**
	 * @param avContext: could be an AVFormatContext or an AVCodecContext.
	 * @param req_flags: filter the AVOption loaded by the Context (default = 0: no flag restriction).
	 * Possible values of flags are:
	 * AV_OPT_FLAG_ENCODING_PARAM
	 * AV_OPT_FLAG_DECODING_PARAM
	 * AV_OPT_FLAG_METADATA
	 * AV_OPT_FLAG_AUDIO_PARAM
	 * AV_OPT_FLAG_VIDEO_PARAM
	 * AV_OPT_FLAG_SUBTITLE_PARAM
	 */
	Context( void* avContext, int req_flags = 0 )
		: _avContext( avContext )
	{
		loadOptions( avContext, req_flags );
	}

	OptionArray getOptions();
	OptionMap& getOptionsMap() { return _options; }

	Option& getOption( const std::string& optionName ) { return _options.at(optionName); }

protected:
	void loadOptions( void* av_class, int req_flags );

protected:
	OptionMap _options;
	void* _avContext;  ///< Has link (no ownership)
};

}

#endif
