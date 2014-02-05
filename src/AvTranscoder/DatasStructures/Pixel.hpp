#ifndef _AV_TRANSCODER_DATA_PIXEL_HPP_
#define _AV_TRANSCODER_DATA_PIXEL_HPP_

#include <string>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/pixfmt.h>
}

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
	: m_bitDepth ( 8 )
	, m_components ( 3 )
	, m_componentType ( eComponentYuv )
	, m_subsamplingType ( eSubsamplingNone )
	, m_endianess ( true )
	, m_outWithAlpha ( false )
	{ }

	void setBitsPerPixel   ( const size_t bitDepth ) { m_bitDepth = bitDepth; }
	void setBigEndian      ( const bool endianess ) { m_endianess = endianess; }
	void setComponents     ( const size_t components ) { m_components = components; }
	void setColorComponents( const EComponentType componentType ) { m_componentType = componentType; }
	void setSubsampling    ( const ESubsamplingType subsamplingType = eSubsamplingNone ) { m_subsamplingType = subsamplingType; }
	void setAlpha          ( const bool outWithAlpha = true ) { m_outWithAlpha = outWithAlpha; }

	bool asCorrectColorComponents( const AVPixFmtDescriptor* pix_desc, const EComponentType componentType );

	bool asCorrectSubsampling( const AVPixFmtDescriptor* pix_desc, const ESubsamplingType subsamplingType );

	void findMatchingPixel();

	AVPixelFormat get(){ return m_pixel; }

private:
	AVPixelFormat    m_pixel;

	size_t           m_bitDepth;
	size_t           m_components;
	double           m_gamma;
	EComponentType   m_componentType;
	ESubsamplingType m_subsamplingType;
	bool             m_endianess;
	bool             m_outWithAlpha;

	//AVChromaLocation
};

}

#endif
