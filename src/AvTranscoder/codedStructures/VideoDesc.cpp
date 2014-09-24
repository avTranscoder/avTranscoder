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
	_codec = essenceDesc.getCodec();
	_codecContext = essenceDesc.getCodecContext();
}

VideoFrameDesc VideoDesc::getVideoFrameDesc() const
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

std::pair< size_t, size_t > VideoDesc::getTimeBase() const
{
	assert( _codecContext != NULL );
	std::pair< size_t, size_t > timeBase;
	timeBase.first = _codecContext->time_base.num;
	timeBase.second = _codecContext->time_base.den;
	return timeBase;
}

void VideoDesc::setImageParameters( const VideoFrameDesc& videoFrameDesc )
{
	setImageParameters( videoFrameDesc.getWidth(), videoFrameDesc.getHeight(), videoFrameDesc.getPixelDesc() );
}


void VideoDesc::setImageParameters( const size_t width, const size_t height, const Pixel& pixel )
{
	setImageParameters( width, height, pixel.findPixel() );
}

void VideoDesc::setImageParameters( const size_t width, const size_t height, const AVPixelFormat& pixel )
{
	_codecContext->width   = width;
	_codecContext->height  = height;
	_codecContext->pix_fmt = pixel;
}

void VideoDesc::setTimeBase( const size_t num, const size_t den, const size_t ticksPerFrame )
{
	_codecContext->time_base.num = num;
	_codecContext->time_base.den = den;
	_codecContext->ticks_per_frame = ticksPerFrame;
}

}
