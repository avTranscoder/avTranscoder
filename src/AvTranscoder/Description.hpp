#ifndef _AV_TRANSCODER_DESCRIPTION_HPP_
#define _AV_TRANSCODER_DESCRIPTION_HPP_

#include <vector>
#include <string>

#include "common.hpp"

namespace avtranscoder
{

class Library
{
public:
	Library( const std::string& name, const std::string& licence, const size_t major, const size_t minor, const size_t release );

	std::string getName();

	std::vector<size_t> getVersion();
	std::string getStringVersion();
	size_t getMajorVersion();
	size_t getMinorVersion();
	size_t getReleaseVersion();

	std::string getLicence();
private:
	std::string _name;
	std::string _licence;
	size_t      _major;
	size_t      _minor;
	size_t      _release;
};

typedef std::vector< Library > Libraries;

Libraries getLibraries();

std::vector<std::string> getInputExtensions();

std::vector<std::string> getOutputExtensions();

}

#endif
