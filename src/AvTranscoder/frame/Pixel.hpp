#ifndef _AV_TRANSCODER_DATA_PIXEL_HPP_
#define _AV_TRANSCODER_DATA_PIXEL_HPP_

#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavutil/pixfmt.h>
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
	eComponentYuvJPEG  ///< YUV color space. 0 <= Y <= 255, 0 <= U, V <= 255
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
	Pixel()
	: _pixelSize  ( 24 )
	, _components ( 3 )
	, _componentType ( eComponentYuv )
	, _subsamplingType ( eSubsamplingNone )
	, _endianess  ( false )
	, _withAlpha  ( false )
	, _planar     ( true )
	{ }

	Pixel( const std::string& avPixelFormat );
	Pixel( const AVPixelFormat avPixelFormat );

	void setBitsPerPixel   ( const size_t pixelSize ) { _pixelSize = pixelSize; }
	void setBigEndian      ( const bool endianess ) { _endianess = endianess; }
	void setComponents     ( const size_t components ) { _components = components; }
	void setColorComponents( const EComponentType componentType ) { _componentType = componentType; }
	void setSubsampling    ( const ESubsamplingType subsamplingType = eSubsamplingNone ) { _subsamplingType = subsamplingType; }
	void setAlpha          ( const bool withAlpha = true ) { _withAlpha = withAlpha; }
	void setPlanar         ( const bool isPlanar ) { _planar = isPlanar; }

	size_t           getBitsPerPixel   () const { return _pixelSize; }
	bool             getBigEndian      () const { return _endianess; }
	size_t           getComponents     () const { return _components; }
	EComponentType   getColorComponents() const { return _componentType; }
	ESubsamplingType getSubsampling    () const { return _subsamplingType; }
	bool             getAlpha          () const { return _withAlpha; }
	bool             getPlanar         () const { return _planar; }

	AVPixelFormat    findPixel() const;

private:
	void init( const AVPixelFormat avPixelFormat );

	bool asCorrectColorComponents( const AVPixFmtDescriptor* pix_desc, const EComponentType componentType ) const;
	bool asCorrectSubsampling( const AVPixFmtDescriptor* pix_desc, const ESubsamplingType subsamplingType ) const;

	size_t           _pixelSize;
	size_t           _components;
	EComponentType   _componentType;
	ESubsamplingType _subsamplingType;
	bool             _endianess;
	bool             _withAlpha;
	bool             _planar;

	//AVChromaLocation
};

}

#endif
