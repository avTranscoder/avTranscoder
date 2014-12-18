#include "VideoTransform.hpp"

#include <AvTranscoder/frame/Pixel.hpp>
#include <AvTranscoder/frame/VideoFrame.hpp>
#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#if LIBAVCODEC_VERSION_MAJOR > 54
	#include <libavutil/frame.h>
#endif
}

#include <iostream>
#include <iomanip>
#include <cassert>
#include <stdexcept>

namespace avtranscoder
{

VideoTransform::VideoTransform()
	: _imageConvertContext( NULL )
	, _srcData     ( (uint8_t)MAX_SWS_PLANE, NULL )
	, _dstData     ( (uint8_t)MAX_SWS_PLANE, NULL )
	, _srcLineSize ( MAX_SWS_PLANE, 0 )
	, _dstLineSize ( MAX_SWS_PLANE, 0 )
	, _isInit      ( false )
	, _verbose( false )
{
}

bool VideoTransform::init( const Frame& srcFrame, const Frame& dstFrame )
{
	const VideoFrame& src = static_cast<const VideoFrame&>( srcFrame );
	const VideoFrame& dst = static_cast<const VideoFrame&>( dstFrame );


	const AVPixelFormat srcPixelFormat = src.desc().getPixel().findPixel();
	const AVPixelFormat dstPixelFormat = dst.desc().getPixel().findPixel();

	_imageConvertContext = sws_getContext(
		src.desc().getWidth(), src.desc().getHeight(), srcPixelFormat,
		dst.desc().getWidth(), dst.desc().getHeight(), dstPixelFormat,
		SWS_POINT, NULL, NULL, NULL);

	if( ! _imageConvertContext )
	{
		throw std::runtime_error( "unable to create color convert context" );
	}

	av_image_fill_linesizes( &_srcLineSize[0], srcPixelFormat, src.desc().getWidth() );
	av_image_fill_linesizes( &_dstLineSize[0], dstPixelFormat, dst.desc().getWidth() );

	if( _verbose )
	{
		std::clog << "video conversion from ";
		const char* pixFmt;
		pixFmt = av_get_pix_fmt_name( srcPixelFormat );
		std::clog << ( pixFmt != NULL ? pixFmt : "None" ) << " to ";
		pixFmt = av_get_pix_fmt_name( dstPixelFormat );
		std::clog << ( pixFmt != NULL ? pixFmt : "None" ) << std::endl;

		std::clog << "source, width = " << src.desc().getWidth() << std::endl;
		std::clog << "source, height = " << src.desc().getHeight() << std::endl;
		
		std::clog << "source, lineSize:" << std::endl;
		std::clog << "[0] = " << _srcLineSize[0] << std::endl;
		std::clog << "[1] = " << _srcLineSize[1] << std::endl;
		std::clog << "[2] = " << _srcLineSize[2] << std::endl;
		std::clog << "[3] = " << _srcLineSize[3] << std::endl;
		
		std::clog << "destination, width = " << dst.desc().getWidth() << std::endl;
		std::clog << "destination, height = " << dst.desc().getHeight() << std::endl;
		
		std::clog << "destination, lineSize:" << std::endl;
		std::clog << "[0] = " << _dstLineSize[0] << std::endl;
		std::clog << "[1] = " << _dstLineSize[1] << std::endl;
		std::clog << "[2] = " << _dstLineSize[2] << std::endl;
		std::clog << "[3] = " << _dstLineSize[3] << std::endl;
	}

	return true;
}

void VideoTransform::convert( const Frame& srcFrame, Frame& dstFrame )
{
	const VideoFrame& src = static_cast<const VideoFrame&>( srcFrame );
	VideoFrame& dst = static_cast<VideoFrame&>( dstFrame );

	assert( src.desc().getWidth()  != 0 );
	assert( src.desc().getHeight() != 0 );
	assert( src.desc().getWidth()  == dst.desc().getWidth()  );
	assert( src.desc().getHeight() == dst.desc().getHeight() );
	assert( src.desc().getPixel().getComponents() != 0 );
	assert( src.desc().getPixel().getComponents() == dst.desc().getPixel().getComponents() );

	if( ! _isInit )
		_isInit = init( srcFrame, dstFrame );

	const AVPixelFormat srcPixelFormat = src.desc().getPixel().findPixel();
	const AVPixelFormat dstPixelFormat = dst.desc().getPixel().findPixel();

	// Fill plane data pointers
	av_image_fill_pointers(&_srcData[0], srcPixelFormat, src.desc().getHeight(), (uint8_t*) src.getPtr(), &_srcLineSize[0]);
	av_image_fill_pointers(&_dstData[0], dstPixelFormat, dst.desc().getHeight(), (uint8_t*) dst.getPtr(), &_dstLineSize[0]);
	
	if( ! _imageConvertContext )
	{
		throw std::runtime_error( "unknown color convert context" );
	}

	if( _verbose )
	{
		std::clog << "source, slice:" << std::endl;
		std::clog << "[0] = " << &_srcData[0] << std::endl;
		std::clog << "[1] = " << &_srcData[1] << std::endl;
		std::clog << "[2] = " << &_srcData[2] << std::endl;
		std::clog << "[3] = " << &_srcData[3] << std::endl;
		
		std::clog << "destination, slice:" << std::endl;
		std::clog << "[0] = " << &_dstData[0] << std::endl;
		std::clog << "[1] = " << &_dstData[1] << std::endl;
		std::clog << "[2] = " << &_dstData[2] << std::endl;
		std::clog << "[3] = " << &_dstData[3] << std::endl;
	}

	int ret = sws_scale( _imageConvertContext,
		&_srcData[0], &_srcLineSize[0], 0, src.desc().getHeight(),
		&_dstData[0], &_dstLineSize[0] );

	if( ret != (int) dst.desc().getHeight() )
		throw std::runtime_error( "error in color converter" );
}

}
