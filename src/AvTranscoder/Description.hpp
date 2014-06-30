#ifndef _AV_TRANSCODER_DESCRIPTION_HPP_
#define _AV_TRANSCODER_DESCRIPTION_HPP_

#include <vector>
#include <string>
#include <map>

#include "common.hpp"

namespace avtranscoder
{

typedef std::map< std::string, std::vector<size_t> > AvVersions;

AvVersions  getVersion();
std::string getLicence();

std::vector<std::string> getInputExtensions();

std::vector<std::string> getOutputExtensions();

}

#endif
