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
	double fps = 1.0 * _avCodecContext->time_base.den / ( _avCodecContext->time_base.num * _avCodecContext->ticks_per_frame );
	if( ! isinf( fps ) )
		videoFrameDesc.setFps( fps );
	return videoFrameDesc;
}

void VideoCodec::setImageParameters( const VideoFrameDesc& videoFrameDesc )
{
	_avCodecContext->width = videoFrameDesc.getWidth();
	_avCodecContext->height = videoFrameDesc.getHeight();
	_avCodecContext->pix_fmt = videoFrameDesc.getPixelFormat();
	_avCodecContext->time_base.num = 1;
	_avCodecContext->time_base.den = videoFrameDesc.getFps();
	_avCodecContext->ticks_per_frame = 1;
}

}
