#ifndef _AV_TRANSCODER_DATA_PIXEL_HPP_
#define _AV_TRANSCODER_DATA_PIXEL_HPP_

#include <string>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
}

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

	bool asCorrectColorComponents( const AVPixFmtDescriptor* pix_desc, const EComponentType componentType )
	{
		if( componentType == eComponentRgb && pix_desc->flags & PIX_FMT_RGB )
			return true;
		if( ( componentType != eComponentRgb ) && ! ( pix_desc->flags & PIX_FMT_RGB ) )
			return true;
		return false;
	}

	bool asCorrectSubsampling( const AVPixFmtDescriptor* pix_desc, const ESubsamplingType subsamplingType )
	{
		switch( subsamplingType )
		{
			case eSubsamplingNone :
				return  ( pix_desc->log2_chroma_w == false ) &&
						( pix_desc->log2_chroma_h == false );
			case eSubsampling422 :
				return  ( pix_desc->log2_chroma_w == true  ) &&
						( pix_desc->log2_chroma_h == false );
			case eSubsampling420 :
				return  ( pix_desc->log2_chroma_w == true  ) &&
						( pix_desc->log2_chroma_h == true  );
		}
		return false;
	}

	void findMatchingPixel()
	{
		const AVPixFmtDescriptor *pix_desc = NULL;
		while( ( pix_desc = av_pix_fmt_desc_next( pix_desc ) ) )
		{
			//std::cout << pix_desc->name << std::endl;
			// enum AVPixelFormat pix_fmt = av_pix_fmt_desc_get_id( pix_desc );
			if( m_components   == (int) pix_desc->nb_components &&
				m_bitDepth     == (size_t) av_get_bits_per_pixel( pix_desc ) &&
				m_endianess    == ( pix_desc->flags & PIX_FMT_BE ) &&
				m_outWithAlpha == ( pix_desc->flags & PIX_FMT_ALPHA ) &&
				asCorrectColorComponents( pix_desc, m_componentType ) &&
				asCorrectSubsampling( pix_desc, m_subsamplingType ) )
			{
				//std::cout << "select : " << pix_desc->name << std::endl;
				m_pixel = av_pix_fmt_desc_get_id( pix_desc );
				return;
			}
		}
	}

	AVPixelFormat operator()(){ return m_pixel; }

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
