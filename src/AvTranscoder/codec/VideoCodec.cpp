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

VideoCodec::VideoCodec( const ECodecType type, AVCodecContext& avCodecContext )
	: ICodec( type, avCodecContext )
{
}

VideoFrameDesc VideoCodec::getVideoFrameDesc() const
{
	assert( _avCodecContext != NULL );
	VideoFrameDesc videoFrameDesc( _avCodecContext->width, _avCodecContext->height, _avCodecContext->pix_fmt );
	return videoFrameDesc;
}

std::pair< size_t, size_t > VideoCodec::getTimeBase() const
{
	assert( _avCodecContext != NULL );
	std::pair< size_t, size_t > timeBase;
	timeBase.first = _avCodecContext->time_base.num;
	timeBase.second = _avCodecContext->time_base.den;
	return timeBase;
}

void VideoCodec::setImageParameters( const VideoFrameDesc& videoFrameDesc )
{
	_avCodecContext->width = videoFrameDesc.getWidth();
	_avCodecContext->height = videoFrameDesc.getHeight();
	_avCodecContext->pix_fmt = videoFrameDesc.getPixelFormat();
}

void VideoCodec::setTimeBase( const size_t num, const size_t den, const size_t ticksPerFrame )
{
	_avCodecContext->time_base.num = num;
	_avCodecContext->time_base.den = den;
	_avCodecContext->ticks_per_frame = ticksPerFrame;
}

}
