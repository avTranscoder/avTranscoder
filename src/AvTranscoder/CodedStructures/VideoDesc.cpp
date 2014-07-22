#include "VideoDesc.hpp"

#include <cassert>

namespace avtranscoder
{

VideoDesc::VideoDesc( const std::string& codecName )
	: CodedDesc( codecName )
{
}

VideoDesc::VideoDesc( const AVCodecID codecId )
	: CodedDesc( codecId )
{
}

VideoDesc::VideoDesc( const CodedDesc& essenceDesc )
	: CodedDesc( essenceDesc.getCodecId() )
{
	m_codec = essenceDesc.getCodec();
	m_codecContext = essenceDesc.getCodecContext();
}

VideoFrameDesc VideoDesc::getVideoFrameDesc() const
{
	assert( m_codecContext != NULL );
	VideoFrameDesc VideoFrameDesc;
	Pixel pixel( m_codecContext->pix_fmt );

	VideoFrameDesc.setWidth ( m_codecContext->width  );
	VideoFrameDesc.setHeight( m_codecContext->height );
	VideoFrameDesc.setPixel ( pixel );
	VideoFrameDesc.setDar   ( m_codecContext->height, m_codecContext->width );
	return VideoFrameDesc;
}

std::pair< size_t, size_t > VideoDesc::getTimeBase() const
{
	assert( m_codecContext != NULL );
	std::pair< size_t, size_t > timeBase;
	timeBase.first = m_codecContext->time_base.num;
	timeBase.second = m_codecContext->time_base.den;
	return timeBase;
}

void VideoDesc::setImageParameters( const VideoFrameDesc& VideoFrameDesc )
{
	setImageParameters( VideoFrameDesc.getWidth(), VideoFrameDesc.getHeight(), VideoFrameDesc.getPixelDesc() );
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
