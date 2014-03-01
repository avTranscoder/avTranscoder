#ifndef _AV_TRANSCODER_DATA_PIXEL_HPP_
#define _AV_TRANSCODER_DATA_PIXEL_HPP_

#include <string>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/pixfmt.h>
}

#include <AvTranscoder/common.hpp>

class AVPixFmtDescriptor;

namespace avtranscoder
{

enum EComponentType
{
	eComponentGray = 0,
	eComponentRgb,
	eComponentYuv
};

enum ESubsamplingType
{
	eSubsamplingNone = 0, // 4:4:4
	eSubsampling422,      // 4:2:2
	eSubsampling420       // 4:2:0
};

class Pixel
{
public:
	Pixel()
	: m_pixelSize  ( 24 )
	, m_components ( 3 )
	, m_componentType ( eComponentYuv )
	, m_subsamplingType ( eSubsamplingNone )
	, m_endianess  ( false )
	, m_withAlpha  ( false )
	, m_planar     ( true )
	{ }

	void setBitsPerPixel   ( const size_t pixelSize ) { m_pixelSize = pixelSize; }
	void setBigEndian      ( const bool endianess ) { m_endianess = endianess; }
	void setComponents     ( const size_t components ) { m_components = components; }
	void setColorComponents( const EComponentType componentType ) { m_componentType = componentType; }
	void setSubsampling    ( const ESubsamplingType subsamplingType = eSubsamplingNone ) { m_subsamplingType = subsamplingType; }
	void setAlpha          ( const bool withAlpha = true ) { m_withAlpha = withAlpha; }
	void setPlanar         ( const bool isPlanar ) { m_planar = isPlanar; }

	size_t           getBitsPerPixel   () const { return m_pixelSize; }
	bool             getBigEndian      () const { return m_endianess; }
	size_t           getComponents     () const { return m_components; }
	EComponentType   getColorComponents() const { return m_componentType; }
	ESubsamplingType getSubsampling    () const { return m_subsamplingType; }
	bool             getAlpha          () const { return m_withAlpha; }
	bool             getPlanar         () const { return m_planar; }

	AVPixelFormat    findPixel() const;

private:
	bool asCorrectColorComponents( const AVPixFmtDescriptor* pix_desc, const EComponentType componentType ) const;
	bool asCorrectSubsampling( const AVPixFmtDescriptor* pix_desc, const ESubsamplingType subsamplingType ) const;

	size_t           m_pixelSize;
	size_t           m_components;
	double           m_gamma;
	EComponentType   m_componentType;
	ESubsamplingType m_subsamplingType;
	bool             m_endianess;
	bool             m_withAlpha;
	bool             m_planar;

	//AVChromaLocation
};

}

#endif
