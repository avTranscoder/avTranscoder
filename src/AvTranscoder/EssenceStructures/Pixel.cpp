#include "Pixel.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>

namespace avtranscoder
{

Pixel::Pixel( const std::string& avPixelFormat )
{
	init( av_get_pix_fmt( avPixelFormat.c_str() ) );
}

Pixel::Pixel( const AVPixelFormat avPixelFormat )
	: m_pixelSize  ( 24 )
	, m_components ( 3 )
	, m_componentType ( eComponentYuv )
	, m_subsamplingType ( eSubsamplingNone )
	, m_endianess  ( false )
	, m_withAlpha  ( false )
	, m_planar     ( true )
{
	init( avPixelFormat );
}

AVPixelFormat Pixel::findPixel() const
{
	//av_register_all();
#if LIBAVUTIL_VERSION_MAJOR > 51
	const AVPixFmtDescriptor *pix_desc = NULL;
	while( ( pix_desc = av_pix_fmt_desc_next( pix_desc ) ) )
	{
#else
	for( size_t pixFmtIndex = PIX_FMT_NONE; pixFmtIndex < PIX_FMT_NB; ++pixFmtIndex )
	{
		const AVPixFmtDescriptor *pix_desc = &av_pix_fmt_descriptors[ pixFmtIndex ];
#endif
		if( m_components   == (size_t) pix_desc->nb_components &&
			m_pixelSize    == (size_t) av_get_bits_per_pixel( pix_desc ) &&
			m_endianess    == ( pix_desc->flags & PIX_FMT_BE ) &&
#if LIBAVUTIL_VERSION_MAJOR > 51
			m_withAlpha    == ( pix_desc->flags & PIX_FMT_ALPHA ) &&
			// @todo what need todo if libavutil <= 51 ?
#endif
			m_planar       == ( ( pix_desc->flags & PIX_FMT_PLANAR ) != 0 ) &&
			asCorrectColorComponents( pix_desc, m_componentType ) &&
			asCorrectSubsampling( pix_desc, m_subsamplingType ) )
		{
#if LIBAVUTIL_VERSION_MAJOR > 51
			return av_pix_fmt_desc_get_id( pix_desc );
#else
			return (AVPixelFormat)pixFmtIndex;
#endif
		}
	}
	return AV_PIX_FMT_NONE;
}

void Pixel::init( const AVPixelFormat avPixelFormat )
{
	const AVPixFmtDescriptor* pix_desc = av_pix_fmt_desc_get( avPixelFormat );
	
	if( ! pix_desc )
	{
		throw std::runtime_error( "unable to find pixel format." ); 
	}
	
	setBitsPerPixel   ( av_get_bits_per_pixel( pix_desc ) );
	setBigEndian      ( pix_desc->flags & PIX_FMT_BE );
	setComponents     ( pix_desc->nb_components );
	setAlpha          ( pix_desc->flags & PIX_FMT_ALPHA );
	setPlanar         ( ( pix_desc->flags & PIX_FMT_PLANAR ) != 0 );

	if( pix_desc->nb_components == 1 )
		setColorComponents( eComponentGray );

	if( pix_desc->flags & PIX_FMT_RGB )
		setColorComponents( eComponentRgb );
	else
		setColorComponents( eComponentYuv );

	setSubsampling( eSubsamplingNone );

	if( ( pix_desc->log2_chroma_w == true ) &&
		( pix_desc->log2_chroma_h == false ) )
	{
		setSubsampling( eSubsampling422 );
	}
	if( ( pix_desc->log2_chroma_w == true ) &&
		( pix_desc->log2_chroma_h == true ) )
	{
		setSubsampling( eSubsampling420 );
	}
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