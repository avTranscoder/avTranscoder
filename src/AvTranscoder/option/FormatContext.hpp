#ifndef _AV_TRANSCODER_FORMAT_CONTEXT_HPP_
#define _AV_TRANSCODER_FORMAT_CONTEXT_HPP_

#include "Context.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

namespace avtranscoder
{

/**
 * @brief Wrapper of an AVFormatContext.
 */
class FormatContext : public Context
{
public:
	FormatContext( AVFormatContext& avFormatContext, int req_flags = 0 );

#ifndef SWIG
	AVFormatContext& getAVFormatContext() const { return *static_cast<AVFormatContext*>( _avContext ); }
#endif
};

}

#endif
