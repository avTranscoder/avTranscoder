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
 * The context could be an AVFormatContext or an AVCodecContext.
 */
class Context
{
public:
	Context( void* avContext, int req_flags = 0 )
		: _avContext( avContext )
	{
		loadOptions( avContext, req_flags );
	}

	Option& getOption( const std::string& optionName ) { return _options.at(optionName); }

private:
	void loadOptions( void* av_class, int req_flags );

private:
	void* _avContext;  ///< Has link (no ownership)
	std::map<std::string, Option> _options;
};

}

#endif
