#include "Pixel.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
}

namespace avtranscoder
{

AVPixelFormat Pixel::findPixel() const
{
	//av_register_all();
	const AVPixFmtDescriptor *pix_desc = NULL;
	while( ( pix_desc = av_pix_fmt_desc_next( pix_desc ) ) )
	{
		if( m_components   == (size_t) pix_desc->nb_components &&
			m_pixelSize    == (size_t) av_get_bits_per_pixel( pix_desc ) &&
			m_endianess    == ( pix_desc->flags & PIX_FMT_BE ) &&
			m_withAlpha    == ( pix_desc->flags & PIX_FMT_ALPHA ) &&
			m_planar       == ( ( pix_desc->flags & PIX_FMT_PLANAR ) != 0 ) &&
			asCorrectColorComponents( pix_desc, m_componentType ) &&
			asCorrectSubsampling( pix_desc, m_subsamplingType ) )
		{
			return av_pix_fmt_desc_get_id( pix_desc );
		}
	}
	return AV_PIX_FMT_NONE;
}

bool Pixel::asCorrectColorComponents( const AVPixFmtDescriptor* pix_desc, const EComponentType componentType ) const 
{
	if( componentType == eComponentRgb && pix_desc->flags & PIX_FMT_RGB )
		return true;
	if( ( componentType != eComponentRgb ) && ( ! ( pix_desc->flags & PIX_FMT_RGB ) ) )
		return true;
	return false;
}

bool Pixel::asCorrectSubsampling( const AVPixFmtDescriptor* pix_desc, const ESubsamplingType subsamplingType ) const
{
	switch( subsamplingType )
	{
		case eSubsamplingNone :
		{
			return  ( pix_desc->log2_chroma_w == false ) &&
					( pix_desc->log2_chroma_h == false );
		}
		case eSubsampling422 :
		{
			return  ( pix_desc->log2_chroma_w == true  ) &&
					( pix_desc->log2_chroma_h == false );
		}
		case eSubsampling420 :
		{
			return  ( pix_desc->log2_chroma_w == true  ) &&
					( pix_desc->log2_chroma_h == true  );
		}
	}
	return false;
}

}