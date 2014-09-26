#ifndef _AV_TRANSCODER_CONTEXT_HPP_
#define _AV_TRANSCODER_CONTEXT_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/option/Option.hpp>

#include <string>
#include <map>

namespace avtranscoder
{

/**
 * @brief Wrapper of AVContext.
 * Can access Options through the corresponding context.
 */
class Context
{
public:
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

	std::vector<Option> getOptions();
	std::map<std::string, Option>& getOptionsMap() { return _options; }

	Option& getOption( const std::string& optionName ) { return _options.at(optionName); }

private:
	void loadOptions( void* av_class, int req_flags );

private:
	std::map<std::string, Option> _options;

protected:
	void* _avContext;  ///< Has link (no ownership)
};

/**
 * @brief Wrapper of an AVFormatContext.
 * @note The AVFormatContext is allocated and free by the class. It is not the case of the base class.
 */
class FormatContext : public Context
{
public:
	FormatContext( int req_flags = 0 );
	~FormatContext();
};

/**
 * @brief Wrapper of an AVCodecContext.
 * @note The AVCodecContext is allocated and free by the class. It is not the case of the base class.
 */
class CodecContext : public Context
{
public:
	CodecContext( int req_flags = 0 );
	~CodecContext();
};

}

#endif
