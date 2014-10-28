#ifndef _MEDIA_MANAGER_CORE_OPTION_CODEC_CONTEXT_HPP_
#define _MEDIA_MANAGER_CORE_OPTION_CODEC_CONTEXT_HPP_

#include "Context.hpp"

struct AVCodecContext;

namespace mediamanager
{
namespace mediacore
{

/**
 * @brief Wrapper of an AVCodecContext.
 * @note The AVCodecContext is allocated and free by the class. It is not the case of the base class.
 */
class MediaManagerExport CodecContext : public Context
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
