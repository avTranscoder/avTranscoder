#ifndef _AV_TRANSCODER_DESCRIPTION_HPP_
#define _AV_TRANSCODER_DESCRIPTION_HPP_

#include <vector>
#include <string>

namespace avtranscoder
{

std::vector<size_t> getVersion();

std::vector<std::string> getInputExtensions();

std::vector<std::string> getOutputExtensions();

}

#endif
