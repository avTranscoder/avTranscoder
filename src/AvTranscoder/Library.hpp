#ifndef _AV_TRANSCODER_DESCRIPTION_HPP_
#define _AV_TRANSCODER_DESCRIPTION_HPP_

#include <AvTranscoder/common.hpp>

#include <vector>
#include <string>

namespace avtranscoder
{

class AvExport Library
{
public:
    Library(const std::string& name, const std::string& license, const size_t major, const size_t minor,
            const size_t release);

    std::string getName();

    std::vector<size_t> getVersion();
    std::string getStringVersion();
    size_t getMajorVersion();
    size_t getMinorVersion();
    size_t getReleaseVersion();

    std::string getLicense();

private:
    std::string _name;
    std::string _licence;
    size_t _major;
    size_t _minor;
    size_t _release;
};

typedef std::vector<Library> Libraries;

Libraries AvExport getLibraries();

std::vector<std::string> AvExport getInputExtensions();

std::vector<std::string> AvExport getOutputExtensions();
}

#endif
