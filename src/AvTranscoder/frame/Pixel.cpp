#include "Pixel.hpp"

extern "C" {
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
{
	init( avPixelFormat );
}

AVPixelFormat Pixel::findPixel() const
{
#if LIBAVUTIL_VERSION_MAJOR > 51
	const AVPixFmtDescriptor *pix_desc = NULL;
	while( ( pix_desc = av_pix_fmt_desc_next( pix_desc ) ) )
	{
#else
	for( size_t pixFmtIndex = PIX_FMT_NONE; pixFmtIndex < PIX_FMT_NB; ++pixFmtIndex )
	{
		const AVPixFmtDescriptor *pix_desc = &av_pix_fmt_descriptors[ pixFmtIndex ];
#endif

		if( _components   == (size_t) pix_desc->nb_components &&
			_pixelSize    == (size_t) av_get_bits_per_pixel( pix_desc ) &&
			_endianess    == ( pix_desc->flags & PIX_FMT_BE ) &&
#if LIBAVUTIL_VERSION_MAJOR > 51
			_withAlpha    == ( pix_desc->flags & PIX_FMT_ALPHA ) &&
			// @todo what need todo if libavutil <= 51 ?
#endif
			_planar       == ( ( pix_desc->flags & PIX_FMT_PLANAR ) != 0 ) &&
			_componentType == getColorComponents( pix_desc ) &&
			_subsamplingType == getSubsampling( pix_desc ) )
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
	setBigEndian      ( ( pix_desc->flags & PIX_FMT_BE ) == PIX_FMT_BE );
	setComponents     ( pix_desc->nb_components );
	setAlpha          ( ( pix_desc->flags & PIX_FMT_ALPHA ) == PIX_FMT_ALPHA );
	setPlanar         ( ( pix_desc->flags & PIX_FMT_PLANAR ) == PIX_FMT_PLANAR );
	setColorComponents( getColorComponents( pix_desc ) );
	setSubsampling( getSubsampling( pix_desc ) );
}

ESubsamplingType Pixel::getSubsampling( const AVPixFmtDescriptor* pix_desc ) const
{
	if( ( pix_desc->log2_chroma_w == 0 ) &&
		( pix_desc->log2_chroma_h == 1 ) )
	{
		return eSubsampling440;
	}
	else if( ( pix_desc->log2_chroma_w == 1 ) &&
		( pix_desc->log2_chroma_h == 0 ) )
	{
		return eSubsampling422;
	}
	else if( ( pix_desc->log2_chroma_w == 1 ) &&
		( pix_desc->log2_chroma_h == 1 ) )
	{
		return eSubsampling420;
	}
	else if( ( pix_desc->log2_chroma_w == 2 ) &&
		( pix_desc->log2_chroma_h == 0 ) )
	{
		return eSubsampling411;
	}
	else if( ( pix_desc->log2_chroma_w == 2 ) &&
		( pix_desc->log2_chroma_h == 2 ) )
	{
		return eSubsampling410;
	}
	else
	{
		return eSubsamplingNone;
	}
}

EComponentType Pixel::getColorComponents( const AVPixFmtDescriptor* pix_desc ) const
{
	if( pix_desc->nb_components == 1 || pix_desc->nb_components == 2 )
	{
        return eComponentGray;
	}
	else if( pix_desc->flags & PIX_FMT_PAL || pix_desc->flags & PIX_FMT_RGB )
	{
		return eComponentRgb;
	}
	else if( pix_desc->name && ! strncmp( pix_desc->name, "yuvj", 4 ) )
	{
		return eComponentYuvJPEG;
	}
	else
	{
		return eComponentYuv;
	}
}

}
