#ifndef _AV_TRANSCODER_COMMON_HPP_
#define _AV_TRANSCODER_COMMON_HPP_

#include <string>

namespace avtranscoder
{

#define MAX_SWS_PLANE 4

struct Ratio
{
	size_t num;
	size_t den;
};

}

#endif
