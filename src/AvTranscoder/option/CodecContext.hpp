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
	CodecContext( AVCodec& avCodec, int req_flags = 0 );  ///< Allocate an AVCodecContext with the given AVCodec
	CodecContext( int req_flags = 0 );  ///< Allocate an AVCodecContext with default values
	~CodecContext();

#ifndef SWIG
	AVCodecContext& getAVCodecContext() const { return *static_cast<AVCodecContext*>( _avContext ); }
#endif
};

}

#endif
