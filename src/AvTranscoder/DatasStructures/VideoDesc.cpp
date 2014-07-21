#include "VideoDesc.hpp"

#include <cassert>

namespace avtranscoder
{

VideoDesc::VideoDesc( const std::string& codecName )
	: EssenceDesc( codecName )
{
}

VideoDesc::VideoDesc( const AVCodecID codecId )
	: EssenceDesc( codecId )
{
}

VideoDesc::VideoDesc( const EssenceDesc& essenceDesc )
	: EssenceDesc( essenceDesc.getCodecId() )
{
	m_codec = essenceDesc.getCodec();
	m_codecContext = essenceDesc.getCodecContext();
}

ImageDesc VideoDesc::getImageDesc() const
{
	assert( m_codecContext != NULL );
	ImageDesc imageDesc;
	Pixel pixel( m_codecContext->pix_fmt );

	imageDesc.setWidth ( m_codecContext->width  );
	imageDesc.setHeight( m_codecContext->height );
	imageDesc.setPixel ( pixel );
	imageDesc.setDar   ( m_codecContext->height, m_codecContext->width );
	return imageDesc;
}

std::pair< size_t, size_t > VideoDesc::getTimeBase() const
{
	assert( m_codecContext != NULL );
	std::pair< size_t, size_t > timeBase;
	timeBase.first = m_codecContext->time_base.num;
	timeBase.second = m_codecContext->time_base.den;
	return timeBase;
}

void VideoDesc::setImageParameters( const ImageDesc& imageDesc )
{
	setImageParameters( imageDesc.getWidth(), imageDesc.getHeight(), imageDesc.getPixelDesc() );
}


void VideoDesc::setImageParameters( const size_t width, const size_t height, const Pixel& pixel )
{
	setImageParameters( width, height, pixel.findPixel() );
}

void VideoDesc::setImageParameters( const size_t width, const size_t height, const AVPixelFormat& pixel )
{
	m_codecContext->width   = width;
	m_codecContext->height  = height;
	m_codecContext->pix_fmt = pixel;
}

void VideoDesc::setTimeBase( const size_t num, const size_t den, const size_t ticksPerFrame )
{
	m_codecContext->time_base.num = num;
	m_codecContext->time_base.den = den;
	m_codecContext->ticks_per_frame = ticksPerFrame;
}

}
