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

std::string Pixel::getPixelName() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	if( _pixelDesc && _pixelDesc->name )
		return std::string( _pixelDesc->name );
	return "unknown pixel name";
}

size_t Pixel::getBitsPerPixel() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return av_get_bits_per_pixel( _pixelDesc );
}

size_t Pixel::getComponents() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return _pixelDesc->nb_components;
}

size_t Pixel::getChromaWidth() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." );

	return _pixelDesc->log2_chroma_w;
}

size_t Pixel::getChromaHeight() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." );

	return _pixelDesc->log2_chroma_h;
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

bool Pixel::isBigEndian() const
{	
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return ( _pixelDesc->flags & PIX_FMT_BE ) == PIX_FMT_BE;
}

bool Pixel::hasAlpha() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

#if LIBAVCODEC_VERSION_MAJOR > 53
	return ( _pixelDesc->flags & PIX_FMT_ALPHA ) == PIX_FMT_ALPHA;
#else
	return false;
#endif
}

bool Pixel::isPlanar() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return ( _pixelDesc->flags & PIX_FMT_PLANAR ) == PIX_FMT_PLANAR;
}

bool Pixel::isIndexedColors() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return ( _pixelDesc->flags & PIX_FMT_PAL ) == PIX_FMT_PAL;
}

bool Pixel::isBitWisePacked() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return ( _pixelDesc->flags & PIX_FMT_BITSTREAM ) == PIX_FMT_BITSTREAM;
}

bool Pixel::isHardwareAccelerated() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return ( _pixelDesc->flags & PIX_FMT_HWACCEL ) == PIX_FMT_HWACCEL;
}

bool Pixel::isRgbPixelData() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	return ( _pixelDesc->flags & PIX_FMT_RGB ) == PIX_FMT_RGB;
}

bool Pixel::isPseudoPaletted() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 


#if LIBAVCODEC_VERSION_MAJOR > 53
	return ( _pixelDesc->flags & PIX_FMT_PSEUDOPAL ) == PIX_FMT_PSEUDOPAL;
#else
	return false;
#endif
}

std::vector<Channel> Pixel::getChannels() const
{
	if( ! _pixelDesc )
		throw std::runtime_error( "unable to find pixel description." ); 

	std::vector<Channel> channels;
	for( size_t channel = 0; channel < (size_t)_pixelDesc->nb_components; ++channel )
	{
		Channel c;
		c.id = channel;
		c.chromaHeight = (size_t)_pixelDesc->comp[channel].plane;
		c.bitStep = (size_t)_pixelDesc->comp[channel].step_minus1;
		channels.push_back( c );
	}
	return channels;
}

PropertiesMap Pixel::getPropertiesAsMap() const
{
	PropertiesMap dataMap;

	detail::add( dataMap, "pixelName", getPixelName() );
	detail::add( dataMap, "bitDepth", getBitsPerPixel() );
	detail::add( dataMap, "nbComponents", getComponents() );
	detail::add( dataMap, "chromaWidth", getChromaWidth() );
	detail::add( dataMap, "chromaHeight", getChromaHeight() );

	std::string colorComponents;
	switch( getColorComponents() )
	{
		case eComponentGray:
			colorComponents = "gray";
			break;
		case eComponentRgb:
			colorComponents = "RGB";
			break;
		case eComponentYuvJPEG:
			colorComponents = "YUVJPEG";
			break;
		case eComponentYuvA:
			colorComponents = "YUVA";
			break;
		case eComponentYuv:
			colorComponents = "YUV";
			break;
	}
	detail::add( dataMap, "colorComponents", colorComponents );

	std::string subsampling;
	switch( getSubsampling() )
	{
		case eSubsampling440:
			subsampling = "440";
			break;
		case eSubsampling422:
			subsampling = "422";
			break;
		case eSubsampling420:
			subsampling = "420";
			break;
		case eSubsampling411:
			subsampling = "411";
			break;
		case eSubsampling410:
			subsampling = "410";
			break;
		case eSubsamplingNone:
			subsampling = "None";
			break;
	}
	detail::add( dataMap, "subsampling", subsampling );

	detail::add( dataMap, "isBigEndian", isBigEndian() );
	detail::add( dataMap, "hasAlpha", hasAlpha() );
	detail::add( dataMap, "isPlanar", isPlanar() );
	detail::add( dataMap, "isIndexedColors", isIndexedColors() );
	detail::add( dataMap, "bitWiseAcked", isBitWisePacked() );
	detail::add( dataMap, "isHardwareAccelerated", isHardwareAccelerated() );
	detail::add( dataMap, "rgbPixel", isRgbPixelData() );
	detail::add( dataMap, "isPseudoPaletted", isPseudoPaletted() );

	std::vector<Channel> channels = getChannels();
	for( size_t channelIndex = 0; channelIndex < channels.size(); ++channelIndex )
	{
		std::stringstream channelName;
		channelName << "channel_" << channels.at( channelIndex ).id;
	
		std::stringstream channelValue;
		channelValue << "chromaHeight " << channels.at( channelIndex ).chromaHeight;
		channelValue << " - ";
		channelValue << "bitStep " << channels.at( channelIndex ).bitStep;

		detail::add( dataMap, channelName.str(), channelValue.str() );
	}

	return dataMap;
}

}
