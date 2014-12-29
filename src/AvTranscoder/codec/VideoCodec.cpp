#include "VideoCodec.hpp"

#include <cassert>

namespace avtranscoder
{

VideoCodec::VideoCodec( const ECodecType type, const std::string& codecName )
	: ICodec( type, codecName )
{
}

VideoCodec::VideoCodec( const ECodecType type, const AVCodecID codecId )
	: ICodec( type, codecId )
{
}

VideoCodec::VideoCodec( const ICodec& codec )
	: ICodec( codec )
{
}

VideoFrameDesc VideoCodec::getVideoFrameDesc() const
{
	assert( _codecContext != NULL );

	VideoFrameDesc videoFrameDesc;
	videoFrameDesc.setWidth ( _codecContext->width  );
	videoFrameDesc.setHeight( _codecContext->height );
	videoFrameDesc.setDar( _codecContext->width, _codecContext->height );

	Pixel pixel( _codecContext->pix_fmt );
	videoFrameDesc.setPixel( pixel );

	return videoFrameDesc;
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
	setImageParameters( videoFrameDesc.getWidth(), videoFrameDesc.getHeight(), videoFrameDesc.getPixel() );
}

void VideoCodec::setImageParameters( const size_t width, const size_t height, const Pixel& pixel )
{
	setImageParameters( width, height, pixel.getAVPixelFormat() );
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
