#ifndef _AV_TRANSCODER_FRAME_PIXEL_HPP_
#define _AV_TRANSCODER_FRAME_PIXEL_HPP_

#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavutil/pixfmt.h>
#include <libavutil/pixdesc.h>
}
#include <string>

struct AVPixFmtDescriptor;

namespace avtranscoder
{

enum EComponentType
{
	eComponentGray = 0,  ///< Gray color space
	eComponentRgb,  ///< RGB color space
	eComponentYuv,  ///< YUV color space. 16 <= Y <= 235, 16 <= U, V <= 240
	eComponentYuvJPEG,  ///< YUV color space. 0 <= Y <= 255, 0 <= U, V <= 255
	eComponentYuvA  ///< YUV color space with transparency
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

class AvExport Pixel
{
public:
	/**
	 * If parameter is an unknown pixel, its format is AV_PIX_FMT_NONE with no access to pixel description.
	 */
	Pixel( const std::string& avPixelFormat = "" );
	Pixel( const AVPixelFormat avPixelFormat );

	size_t           getBitsPerPixel   () const;
	bool             getBigEndian      () const;
	size_t           getComponents     () const;
	EComponentType   getColorComponents() const;
	ESubsamplingType getSubsampling    () const;
	bool             getAlpha          () const;
	bool             getPlanar         () const;

	AVPixelFormat    findPixel() const;

private:
	void init( const AVPixelFormat avPixelFormat );

private:
	AVPixelFormat _pixelFormat;
	const AVPixFmtDescriptor* _pixelDesc;
};

}

#endif
