#include "VideoCodec.hpp"

#include <cassert>

namespace avtranscoder
{

VideoCodec::VideoCodec( const std::string& codecName )
	: ICodec( codecName )
{
}

VideoCodec::VideoCodec( const AVCodecID codecId )
	: ICodec( codecId )
{
}

VideoCodec::VideoCodec( const ICodec& essenceDesc )
	: ICodec( essenceDesc.getCodecId() )
{
	_codec = essenceDesc.getAVCodec();
	_codecContext = essenceDesc.getAVCodecContext();
}

VideoFrameDesc VideoCodec::getVideoFrameDesc() const
{
	assert( _codecContext != NULL );
	VideoFrameDesc VideoFrameDesc;
	Pixel pixel( _codecContext->pix_fmt );

	VideoFrameDesc.setWidth ( _codecContext->width  );
	VideoFrameDesc.setHeight( _codecContext->height );
	VideoFrameDesc.setPixel ( pixel );
	VideoFrameDesc.setDar   ( _codecContext->height, _codecContext->width );
	return VideoFrameDesc;
}

std::pair< size_t, size_t > VideoCodec::getTimeBase() const
{
	assert( _codecContext != NULL );
	std::pair< size_t, size_t > timeBase;
	timeBase.first = _codecContext->time_base.num;
	timeBase.second = _codecContext->time_base.den;
	return timeBase;
}

void VideoCodec::setImageParameters( const VideoFrameDesc& videoFrameDesc )
{
	setImageParameters( videoFrameDesc.getWidth(), videoFrameDesc.getHeight(), videoFrameDesc.getPixelDesc() );
}


void VideoCodec::setImageParameters( const size_t width, const size_t height, const Pixel& pixel )
{
	setImageParameters( width, height, pixel.findPixel() );
}

void VideoCodec::setImageParameters( const size_t width, const size_t height, const AVPixelFormat& pixel )
{
	_codecContext->width   = width;
	_codecContext->height  = height;
	_codecContext->pix_fmt = pixel;
}

void VideoCodec::setTimeBase( const size_t num, const size_t den, const size_t ticksPerFrame )
{
	_codecContext->time_base.num = num;
	_codecContext->time_base.den = den;
	_codecContext->ticks_per_frame = ticksPerFrame;
}

}
