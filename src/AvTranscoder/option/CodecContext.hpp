#ifndef _AV_TRANSCODER_CODEC_CONTEXT_HPP_
#define _AV_TRANSCODER_CODEC_CONTEXT_HPP_

#include "Context.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace avtranscoder
{

/**
 * @brief Wrapper of an AVCodecContext.
 */
class AvExport  CodecContext : public Context
{
public:
	CodecContext( AVCodecContext& avCodecContext, int req_flags = 0 );

#ifndef SWIG
	AVCodecContext& getAVCodecContext() const { return *static_cast<AVCodecContext*>( _avContext ); }
#endif
};

}

#endif
