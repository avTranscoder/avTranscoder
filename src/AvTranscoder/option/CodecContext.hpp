#ifndef _AV_TRANSCODER_CODEC_CONTEXT_HPP_
#define _AV_TRANSCODER_CODEC_CONTEXT_HPP_

#include "Context.hpp"

struct AVCodecContext;

namespace avtranscoder
{

/**
 * @brief Wrapper of an AVCodecContext.
 * @note The AVCodecContext is allocated and free by the class. It is not the case of the base class.
 */
class CodecContext : public Context
{
public:
	CodecContext( int req_flags = 0 );
	~CodecContext();
	
private:
	AVCodecContext* _avCodecContext;
};

}

#endif
