#ifndef _AV_TRANSCODER_FORMAT_CONTEXT_HPP_
#define _AV_TRANSCODER_FORMAT_CONTEXT_HPP_

#include "Context.hpp"

struct AVFormatContext;

namespace avtranscoder
{

/**
 * @brief Wrapper of an AVFormatContext.
 * @note The AVFormatContext is allocated and free by the class. It is not the case of the base class.
 */
class FormatContext : public Context
{
public:
	FormatContext( int req_flags = 0 );
	~FormatContext();

private:
	AVFormatContext* _avFormatContext;
};

}

#endif
