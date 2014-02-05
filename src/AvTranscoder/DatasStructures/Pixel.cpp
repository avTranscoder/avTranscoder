#include "Pixel.hpp"

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

bool Pixel::asCorrectColorComponents( const AVPixFmtDescriptor* pix_desc, const EComponentType componentType )
{
	if( componentType == eComponentRgb && pix_desc->flags & PIX_FMT_RGB )
		return true;
	if( ( componentType != eComponentRgb ) && ! ( pix_desc->flags & PIX_FMT_RGB ) )
		return true;
	return false;
}

bool Pixel::asCorrectSubsampling( const AVPixFmtDescriptor* pix_desc, const ESubsamplingType subsamplingType )
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

void Pixel::findMatchingPixel()
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

}