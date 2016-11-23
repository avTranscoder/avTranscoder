#include "PixelProperties.hpp"

#include <stdexcept>
#include <cstring>

namespace avtranscoder
{

PixelProperties::PixelProperties(const std::string& avPixelFormat)
    : _pixelFormat(AV_PIX_FMT_NONE)
    , _pixelDesc(NULL)
{
    init(av_get_pix_fmt(avPixelFormat.c_str()));
}

PixelProperties::PixelProperties(const AVPixelFormat avPixelFormat)
    : _pixelFormat(AV_PIX_FMT_NONE)
    , _pixelDesc(NULL)
{
    init(avPixelFormat);
}

void PixelProperties::init(const AVPixelFormat avPixelFormat)
{
    _pixelFormat = avPixelFormat;
    _pixelDesc = av_pix_fmt_desc_get(avPixelFormat);
}

std::string PixelProperties::getPixelName() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");

    if(!_pixelDesc || !_pixelDesc->name)
        throw std::runtime_error("unknown pixel name");

    return std::string(_pixelDesc->name);
}

std::string PixelProperties::getPixelFormatName() const
{
    if(_pixelFormat == AV_PIX_FMT_NONE)
        throw std::runtime_error("unable to find pixel format.");

    const char* formatName = av_get_pix_fmt_name(_pixelFormat);
    if(!formatName)
        throw std::runtime_error("unknown pixel format");

    return std::string(formatName);
}

size_t PixelProperties::getBitsPerPixel() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");
    return av_get_bits_per_pixel(_pixelDesc);
}

size_t PixelProperties::getMaxNbBitsInChannels() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");

    size_t maxNbBitsInChannels = 0;
    for(unsigned int channelIndex = 0; channelIndex < _pixelDesc->nb_components; ++channelIndex)
    {
        const size_t nbBits = _pixelDesc->comp[channelIndex].depth_minus1 + 1;
        if(nbBits > maxNbBitsInChannels)
            maxNbBitsInChannels = nbBits;
    }
    return maxNbBitsInChannels;
}

size_t PixelProperties::getNbComponents() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");
    return _pixelDesc->nb_components;
}

size_t PixelProperties::getChromaWidth() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");
    return _pixelDesc->log2_chroma_w;
}

size_t PixelProperties::getChromaHeight() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");
    return _pixelDesc->log2_chroma_h;
}

EComponentType PixelProperties::getColorComponents() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");

    if(_pixelDesc->nb_components == 1 || _pixelDesc->nb_components == 2)
    {
        return eComponentGray;
    }
    else if(_pixelDesc->flags & AV_PIX_FMT_FLAG_PAL || _pixelDesc->flags & AV_PIX_FMT_FLAG_RGB)
    {
        return eComponentRgb;
    }
    else if(_pixelDesc->name && !strncmp(_pixelDesc->name, "yuvj", 4))
    {
        return eComponentYuvJPEG;
    }
    else if(_pixelDesc->name && !strncmp(_pixelDesc->name, "yuva", 4))
    {
        return eComponentYuvA;
    }
    else
    {
        return eComponentYuv;
    }
}

ESubsamplingType PixelProperties::getSubsampling() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");

    if((_pixelDesc->log2_chroma_w == 0) && (_pixelDesc->log2_chroma_h == 1))
    {
        return eSubsampling440;
    }
    else if((_pixelDesc->log2_chroma_w == 1) && (_pixelDesc->log2_chroma_h == 0))
    {
        return eSubsampling422;
    }
    else if((_pixelDesc->log2_chroma_w == 1) && (_pixelDesc->log2_chroma_h == 1))
    {
        return eSubsampling420;
    }
    else if((_pixelDesc->log2_chroma_w == 2) && (_pixelDesc->log2_chroma_h == 0))
    {
        return eSubsampling411;
    }
    else if((_pixelDesc->log2_chroma_w == 2) && (_pixelDesc->log2_chroma_h == 2))
    {
        return eSubsampling410;
    }
    else
    {
        return eSubsamplingNone;
    }
}

bool PixelProperties::isBigEndian() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");
    return (_pixelDesc->flags & AV_PIX_FMT_FLAG_BE) == AV_PIX_FMT_FLAG_BE;
}

bool PixelProperties::hasAlpha() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");

#if LIBAVCODEC_VERSION_MAJOR > 53
    return (_pixelDesc->flags & AV_PIX_FMT_FLAG_ALPHA) == AV_PIX_FMT_FLAG_ALPHA;
#else
    return false;
#endif
}

bool PixelProperties::isPlanar() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");
    return (_pixelDesc->flags & AV_PIX_FMT_FLAG_PLANAR) == AV_PIX_FMT_FLAG_PLANAR;
}

bool PixelProperties::isIndexedColors() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");
    return (_pixelDesc->flags & AV_PIX_FMT_FLAG_PAL) == AV_PIX_FMT_FLAG_PAL;
}

bool PixelProperties::isBitWisePacked() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");
    return (_pixelDesc->flags & AV_PIX_FMT_FLAG_BITSTREAM) == AV_PIX_FMT_FLAG_BITSTREAM;
}

bool PixelProperties::isHardwareAccelerated() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");
    return (_pixelDesc->flags & AV_PIX_FMT_FLAG_HWACCEL) == AV_PIX_FMT_FLAG_HWACCEL;
}

bool PixelProperties::isRgbPixelData() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");
    return (_pixelDesc->flags & AV_PIX_FMT_FLAG_RGB) == AV_PIX_FMT_FLAG_RGB;
}

bool PixelProperties::isPseudoPaletted() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");

#if LIBAVCODEC_VERSION_MAJOR > 53
    return (_pixelDesc->flags & AV_PIX_FMT_FLAG_PSEUDOPAL) == AV_PIX_FMT_FLAG_PSEUDOPAL;
#else
    return false;
#endif
}

std::vector<Channel> PixelProperties::getChannels() const
{
    if(!_pixelDesc)
        throw std::runtime_error("unable to find pixel description.");

    std::vector<Channel> channels;
    for(size_t channel = 0; channel < (size_t)_pixelDesc->nb_components; ++channel)
    {
        Channel c;
        c.id = channel;
        c.chromaHeight = (size_t)_pixelDesc->comp[channel].plane;
        c.bitStep = (size_t)_pixelDesc->comp[channel].step_minus1;
        channels.push_back(c);
    }
    return channels;
}

PropertyVector PixelProperties::asVector() const
{
    PropertyVector propertyVector;
    return fillVector(propertyVector);
}

PropertyVector& PixelProperties::fillVector(PropertyVector& data) const
{
    detail::addProperty(data, "pixelName", this, &PixelProperties::getPixelName);
    detail::addProperty(data, "pixelFormatName", this, &PixelProperties::getPixelFormatName);
    detail::addProperty(data, "bitDepth", this, &PixelProperties::getBitsPerPixel);
    detail::addProperty(data, "maxNbBitsInChannels", this, &PixelProperties::getMaxNbBitsInChannels);
    detail::addProperty(data, "nbComponents", this, &PixelProperties::getNbComponents);
    detail::addProperty(data, "chromaWidth", this, &PixelProperties::getChromaWidth);
    detail::addProperty(data, "chromaHeight", this, &PixelProperties::getChromaHeight);

    try
    {
        std::string colorComponents;
        switch(getColorComponents())
        {
            case eComponentGray:
                colorComponents = "gray";
                break;
            case eComponentRgb:
                colorComponents = "RGB";
                break;
            case eComponentYuvJPEG:
                colorComponents = "YUVJPEG";
                break;
            case eComponentYuvA:
                colorComponents = "YUVA";
                break;
            case eComponentYuv:
                colorComponents = "YUV";
                break;
        }
        detail::add(data, "colorComponents", colorComponents);
    }
    catch(const std::exception& e)
    {
        detail::add(data, "colorComponents", detail::propertyValueIfError);
    }

    try
    {
        std::string subsampling;
        switch(getSubsampling())
        {
            case eSubsampling440:
                subsampling = "440";
                break;
            case eSubsampling422:
                subsampling = "422";
                break;
            case eSubsampling420:
                subsampling = "420";
                break;
            case eSubsampling411:
                subsampling = "411";
                break;
            case eSubsampling410:
                subsampling = "410";
                break;
            case eSubsamplingNone:
                subsampling = "None";
                break;
        }
        detail::add(data, "subsampling", subsampling);
    }
    catch(const std::exception& e)
    {
        detail::add(data, "subsampling", detail::propertyValueIfError);
    }

    detail::addProperty(data, "isBigEndian", this, &PixelProperties::isBigEndian);
    detail::addProperty(data, "hasAlpha", this, &PixelProperties::hasAlpha);
    detail::addProperty(data, "isPlanar", this, &PixelProperties::isPlanar);
    detail::addProperty(data, "isIndexedColors", this, &PixelProperties::isIndexedColors);
    detail::addProperty(data, "bitWiseAcked", this, &PixelProperties::isBitWisePacked);
    detail::addProperty(data, "isHardwareAccelerated", this, &PixelProperties::isHardwareAccelerated);
    detail::addProperty(data, "rgbPixel", this, &PixelProperties::isRgbPixelData);
    detail::addProperty(data, "isPseudoPaletted", this, &PixelProperties::isPseudoPaletted);

    try
    {
        std::vector<Channel> channels = getChannels();
        for(size_t channelIndex = 0; channelIndex < channels.size(); ++channelIndex)
        {
            std::stringstream channelName;
            channelName << "channel_" << channels.at(channelIndex).id;

            std::stringstream channelValue;
            channelValue << "chromaHeight " << channels.at(channelIndex).chromaHeight;
            channelValue << " - ";
            channelValue << "bitStep " << channels.at(channelIndex).bitStep;

            detail::add(data, channelName.str(), channelValue.str());
        }
    }
    catch(const std::exception& e)
    {
        detail::add(data, "channels", detail::propertyValueIfError);
    }

    return data;
}
}
