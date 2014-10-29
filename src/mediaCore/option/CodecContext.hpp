#ifndef _MEDIA_ENGINE_CORE_OPTION_CODEC_CONTEXT_HPP_
#define _MEDIA_ENGINE_CORE_OPTION_CODEC_CONTEXT_HPP_

#include "Context.hpp"

struct AVCodecContext;

namespace mediaengine
{
namespace mediacore
{

/**
 * @brief Wrapper of an AVCodecContext.
 * @note The AVCodecContext is allocated and free by the class. It is not the case of the base class.
 */
class MediaEngineExport CodecContext : public Context
{
public:
	CodecContext( int req_flags = 0 );
	~CodecContext();
	
private:
	AVCodecContext* _avCodecContext;
};

}
}

#endif
