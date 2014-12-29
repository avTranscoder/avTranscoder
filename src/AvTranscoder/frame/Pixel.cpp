#include "Pixel.hpp"

#include <stdexcept>

namespace avtranscoder
{

Pixel::Pixel( const std::string& avPixelFormat )
	: _pixelFormat( AV_PIX_FMT_NONE )
	,_pixelDesc( NULL )
{
	init( av_get_pix_fmt( avPixelFormat.c_str() ) );
}

Pixel::Pixel( const AVPixelFormat avPixelFormat )
	: _pixelFormat( AV_PIX_FMT_NONE )
	,_pixelDesc( NULL )
{
	init( avPixelFormat );
}

void Pixel::init( const AVPixelFormat avPixelFormat )
{
	_pixelFormat = avPixelFormat;
	_pixelDesc = av_pix_fmt_desc_get( avPixelFormat );
}

size_t Pixel::getBitsPerPixel() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return av_get_bits_per_pixel( _pixelDesc );
}

bool Pixel::getBigEndian() const
{	
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return ( _pixelDesc->flags & PIX_FMT_BE ) == PIX_FMT_BE;
}

size_t Pixel::getComponents() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return _pixelDesc->nb_components;
}

EComponentType Pixel::getColorComponents() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	if( _pixelDesc->nb_components == 1 || _pixelDesc->nb_components == 2 )
	{
        return eComponentGray;
	}
	else if( _pixelDesc->flags & PIX_FMT_PAL || _pixelDesc->flags & PIX_FMT_RGB )
	{
		return eComponentRgb;
	}
	else if( _pixelDesc->name && ! strncmp( _pixelDesc->name, "yuvj", 4 ) )
	{
		return eComponentYuvJPEG;
	}
	else if( _pixelDesc->name && ! strncmp( _pixelDesc->name, "yuva", 4 ) )
	{
		return eComponentYuvA;
	}
	else
	{
		return eComponentYuv;
	}
}

ESubsamplingType Pixel::getSubsampling() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	if( ( _pixelDesc->log2_chroma_w == 0 ) &&
		( _pixelDesc->log2_chroma_h == 1 ) )
	{
		return eSubsampling440;
	}
	else if( ( _pixelDesc->log2_chroma_w == 1 ) &&
		( _pixelDesc->log2_chroma_h == 0 ) )
	{
		return eSubsampling422;
	}
	else if( ( _pixelDesc->log2_chroma_w == 1 ) &&
		( _pixelDesc->log2_chroma_h == 1 ) )
	{
		return eSubsampling420;
	}
	else if( ( _pixelDesc->log2_chroma_w == 2 ) &&
		( _pixelDesc->log2_chroma_h == 0 ) )
	{
		return eSubsampling411;
	}
	else if( ( _pixelDesc->log2_chroma_w == 2 ) &&
		( _pixelDesc->log2_chroma_h == 2 ) )
	{
		return eSubsampling410;
	}
	else
	{
		return eSubsamplingNone;
	}
}

bool Pixel::getAlpha() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return ( _pixelDesc->flags & PIX_FMT_ALPHA ) == PIX_FMT_ALPHA;
}

bool Pixel::getPlanar() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return ( _pixelDesc->flags & PIX_FMT_PLANAR ) == PIX_FMT_PLANAR;
}

AVPixelFormat Pixel::findPixel() const
{
	return _pixelFormat;
}

}
