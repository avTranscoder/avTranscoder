#include "Library.hpp"

extern "C" {
#include <libavutil/version.h>
#include <libavcodec/version.h>
#include <libswscale/version.h>
#include <libswscale/swscale.h>
#ifdef AVTRANSCODER_LIBAV_DEPENDENCY
#include <libavresample/version.h>
#else
#include <libswresample/version.h>
#endif
#include <libavformat/avformat.h>
}

#include <cstring>
#include <sstream>
#include <algorithm>

namespace avtranscoder
{

Library::Library(const std::string& name, const std::string& license, const size_t major, const size_t minor,
                 const size_t release)
    : _name(name)
    , _licence(license)
    , _major(major)
    , _minor(minor)
    , _release(release)
{
}

std::string Library::getName()
{
    return _name;
}

std::vector<size_t> Library::getVersion()
{
    std::vector<size_t> version;
    version.push_back(_major);
    version.push_back(_minor);
    version.push_back(_release);
    return version;
}

std::string Library::getStringVersion()
{
    std::stringstream version;
    version << _major << ".";
    version << _minor << ".";
    version << _release;
    return version.str();
}

size_t Library::getMajorVersion()
{
    return _major;
}

size_t Library::getMinorVersion()
{
    return _minor;
}

size_t Library::getReleaseVersion()
{
    return _release;
}

std::string Library::getLicense()
{
    return _licence;
}

Libraries getLibraries()
{
    Libraries libs;

    libs.push_back(Library("avtranscoder", "GPL v2 or LGPL v2.1", AVTRANSCODER_VERSION_MAJOR, AVTRANSCODER_VERSION_MINOR,
                           AVTRANSCODER_VERSION_MICRO));
    libs.push_back(
        Library("avutil", avutil_license(), LIBAVUTIL_VERSION_MAJOR, LIBAVUTIL_VERSION_MINOR, LIBAVUTIL_VERSION_MICRO));
    libs.push_back(Library("avformat", avformat_license(), LIBAVFORMAT_VERSION_MAJOR, LIBAVFORMAT_VERSION_MINOR,
                           LIBAVFORMAT_VERSION_MICRO));
    libs.push_back(
        Library("avcodec", avcodec_license(), LIBAVCODEC_VERSION_MAJOR, LIBAVCODEC_VERSION_MINOR, LIBAVCODEC_VERSION_MICRO));
#ifdef AVTRANSCODER_LIBAV_DEPENDENCY
    libs.push_back(Library("avresample", avutil_license(), LIBAVRESAMPLE_VERSION_MAJOR, LIBAVRESAMPLE_VERSION_MINOR,
                           LIBAVRESAMPLE_VERSION_MICRO));
#else
    libs.push_back(Library("swresample", avutil_license(), LIBSWRESAMPLE_VERSION_MAJOR, LIBSWRESAMPLE_VERSION_MINOR,
                           LIBSWRESAMPLE_VERSION_MICRO));
#endif
    libs.push_back(
        Library("swscale", swscale_license(), LIBSWSCALE_VERSION_MAJOR, LIBSWSCALE_VERSION_MINOR, LIBSWSCALE_VERSION_MICRO));

    return libs;
}

std::vector<std::string> getInputExtensions()
{
    std::vector<std::string> extensions;
    AVInputFormat* iFormat = NULL;

    while((iFormat = av_iformat_next(iFormat)))
    {
        if(iFormat->extensions != NULL)
        {
            // parse extensions
            std::string exts = std::string(iFormat->extensions);
            char* ext = strtok(const_cast<char*>(exts.c_str()), ",");
            while(ext != NULL)
            {
                extensions.push_back(std::string(ext));
                ext = strtok(NULL, ",");
            }

            // parse name (name's format defines (in general) extensions )
            // don't need to do it in recent LibAV/FFMpeg versions
            exts = std::string(iFormat->name);
            ext = strtok(const_cast<char*>(exts.c_str()), ",");
            while(ext != NULL)
            {
                extensions.push_back(std::string(ext));
                ext = strtok(NULL, ",");
            }
        }
    }
    // sort
    std::sort(extensions.begin(), extensions.end());
    // suppress duplicates
    std::vector<std::string>::iterator last = std::unique(extensions.begin(), extensions.end());
    extensions.erase(last, extensions.end());

    return extensions;
}

std::vector<std::string> getOutputExtensions()
{
    std::vector<std::string> extensions;
    AVOutputFormat* oFormat = NULL;

    while((oFormat = av_oformat_next(oFormat)))
    {
        if(oFormat->extensions != NULL)
        {
            // parse extensions
            std::string exts = std::string(oFormat->extensions);
            char* ext = strtok(const_cast<char*>(exts.c_str()), ",");
            while(ext != NULL)
            {
                extensions.push_back(std::string(ext));
                ext = strtok(NULL, ",");
            }

            // parse name (name's format defines (in general) extensions )
            // don't need to do it in recent LibAV/FFMpeg versions
            exts = std::string(oFormat->name);
            ext = strtok(const_cast<char*>(exts.c_str()), ",");
            while(ext != NULL)
            {
                extensions.push_back(std::string(ext));
                ext = strtok(NULL, ",");
            }
        }
    }
    // sort
    std::sort(extensions.begin(), extensions.end());
    // suppress duplicates
    std::vector<std::string>::iterator last = std::unique(extensions.begin(), extensions.end());
    extensions.erase(last, extensions.end());

    return extensions;
}
}
