#ifndef _AV_TRANSCODER_FRAME_PIXEL_PROPERTIES_HPP_
#define _AV_TRANSCODER_FRAME_PIXEL_PROPERTIES_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/properties/util.hpp>

extern "C" {
#include <libavutil/pixfmt.h>
#include <libavutil/pixdesc.h>
}

#include <string>
#include <vector>

struct AVPixFmtDescriptor;

namespace avtranscoder
{

enum EComponentType
{
    eComponentGray = 0, ///< Gray color space
    eComponentRgb,      ///< RGB color space
    eComponentYuv,      ///< YUV color space. 16 <= Y <= 235, 16 <= U, V <= 240
    eComponentYuvJPEG,  ///< YUV color space. 0 <= Y <= 255, 0 <= U, V <= 255
    eComponentYuvA      ///< YUV color space with transparency
};

enum ESubsamplingType
{
    eSubsamplingNone = 0, // 4:4:4
    eSubsampling440,      // 4:4:0
    eSubsampling422,      // 4:2:2
    eSubsampling420,      // 4:2:0
    eSubsampling411,      // 4:1:1
    eSubsampling410       // 4:1:0
};

struct AvExport Channel
{
    size_t id;
    size_t chromaHeight;
    size_t bitStep;
};

class AvExport PixelProperties
{
public:
    /**
     * If parameter is an unknown pixel, its format is AV_PIX_FMT_NONE with no access to pixel description.
     */
    PixelProperties(const std::string& avPixelFormat = "");
    PixelProperties(const AVPixelFormat avPixelFormat);

    std::string getPixelName() const;
    std::string getPixelFormatName() const;

    size_t getBitsPerPixel() const; ///< padding bits are not counted
    size_t getMaxNbBitsInChannels() const;
    size_t getNbComponents() const;
    size_t getChromaWidth() const;
    size_t getChromaHeight() const;

    EComponentType getColorComponents() const;
    ESubsamplingType getSubsampling() const;

    bool isBigEndian() const;
    bool hasAlpha() const;
    bool isPlanar() const;
    bool isIndexedColors() const;
    bool isBitWisePacked() const;
    bool isHardwareAccelerated() const;
    bool isRgbPixelData() const;
    bool isPseudoPaletted() const;

    std::vector<Channel> getChannels() const;

#ifndef SWIG
    AVPixelFormat getAVPixelFormat() const { return _pixelFormat; }
    const AVPixFmtDescriptor* getAVPixFmtDescriptor() const { return _pixelDesc; }
#endif

    PropertyVector asVector() const; ///< Return all pixel properties as a vector (name of property: value)
    PropertyVector& fillVector(PropertyVector& data) const; ///< To avoid copy of the vector

private:
    void init(const AVPixelFormat avPixelFormat);

private:
    AVPixelFormat _pixelFormat;
    const AVPixFmtDescriptor* _pixelDesc; ///< Has link (no ownership)
};
}

#endif
