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

	VideoFrameDesc videoFrameDesc( _codecContext->getAVCodecContext().width, _codecContext->getAVCodecContext().height, _codecContext->getAVCodecContext().pix_fmt );
	return videoFrameDesc;
}

std::pair< size_t, size_t > VideoCodec::getTimeBase() const
{
	assert( _codecContext != NULL );
	std::pair< size_t, size_t > timeBase;
	timeBase.first = _codecContext->getAVCodecContext().time_base.num;
	timeBase.second = _codecContext->getAVCodecContext().time_base.den;
	return timeBase;
}

void VideoCodec::setImageParameters( const VideoFrameDesc& videoFrameDesc )
{
	setImageParameters( videoFrameDesc.getWidth(), videoFrameDesc.getHeight(), videoFrameDesc.getPixelFormat() );
}

void VideoCodec::setImageParameters( const size_t width, const size_t height, const AVPixelFormat& pixel )
{
	_codecContext->getAVCodecContext().width   = width;
	_codecContext->getAVCodecContext().height  = height;
	_codecContext->getAVCodecContext().pix_fmt = pixel;
}

void VideoCodec::setTimeBase( const size_t num, const size_t den, const size_t ticksPerFrame )
{
	_codecContext->getAVCodecContext().time_base.num = num;
	_codecContext->getAVCodecContext().time_base.den = den;
	_codecContext->getAVCodecContext().ticks_per_frame = ticksPerFrame;
}

}
