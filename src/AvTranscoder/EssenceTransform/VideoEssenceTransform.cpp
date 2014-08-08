#include "VideoEssenceTransform.hpp"

#include <AvTranscoder/EssenceStructures/Pixel.hpp>
#include <AvTranscoder/EssenceStructures/VideoFrame.hpp>
#include <AvTranscoder/common.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
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

VideoEssenceTransform::VideoEssenceTransform()
	: _imageConvertContext( NULL )
	, _srcData     ( (uint8_t)MAX_SWS_PLANE, NULL )
	, _dstData     ( (uint8_t)MAX_SWS_PLANE, NULL )
	, _srcLineSize ( MAX_SWS_PLANE, 0 )
	, _dstLineSize ( MAX_SWS_PLANE, 0 )
	, _srcOffsets  ( MAX_SWS_PLANE, 0 )
	, _dstOffsets  ( MAX_SWS_PLANE, 0 )
	, _isInit      ( false )
	, _verbose( false )
{
}

bool VideoEssenceTransform::init( const Frame& srcFrame, const Frame& dstFrame )
{
	const VideoFrame& src = static_cast<const VideoFrame&>( srcFrame );
	const VideoFrame& dst = static_cast<const VideoFrame&>( dstFrame );

	assert( src.desc().getWidth()  != 0 );
	assert( src.desc().getHeight() != 0 );
	assert( src.desc().getWidth()  == dst.desc().getWidth()  );
	assert( src.desc().getHeight() == dst.desc().getHeight() );

	_imageConvertContext = sws_getContext(
		src.desc().getWidth(), src.desc().getHeight(), src.desc().getPixelDesc().findPixel(),
		dst.desc().getWidth(), dst.desc().getHeight(), dst.desc().getPixelDesc().findPixel(),
		SWS_POINT, NULL, NULL, NULL);

	if( ! _imageConvertContext )
	{
		throw std::runtime_error( "unable to create color convert context" );
	}

	av_image_fill_linesizes( &_srcLineSize[0], src.desc().getPixelDesc().findPixel(), src.desc().getWidth() );
	av_image_fill_linesizes( &_dstLineSize[0], dst.desc().getPixelDesc().findPixel(), dst.desc().getWidth() );

	if( _verbose )
	{
		std::clog << "video conversion from ";
		const char* pixFmt;
		pixFmt = av_get_pix_fmt_name( src.desc().getPixelDesc().findPixel() );
		std::clog << ( pixFmt != NULL ? pixFmt : "None" ) << " to ";
		pixFmt = av_get_pix_fmt_name( dst.desc().getPixelDesc().findPixel() );
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

	size_t cumulSrcOffset = 0;
	size_t cumulDstOffset = 0;

	for( size_t plane = 0; plane < MAX_SWS_PLANE; ++plane )
	{
#ifdef FFALIGN
		_srcLineSize.at( plane ) = FFALIGN( _srcLineSize.at( plane ), 16 );
		_dstLineSize.at( plane ) = FFALIGN( _dstLineSize.at( plane ), 16 );
#else
		_srcLineSize.at( plane ) = _srcLineSize.at( plane );
		_dstLineSize.at( plane ) = _dstLineSize.at( plane );
#endif
		_srcOffsets.at( plane ) = cumulSrcOffset;
		_dstOffsets.at( plane ) = cumulDstOffset;

		cumulSrcOffset += _srcLineSize.at( plane ) * src.desc().getHeight();
		cumulDstOffset += _dstLineSize.at( plane ) * dst.desc().getHeight();
	}

	return true;
}

void VideoEssenceTransform::convert( const Frame& srcFrame, Frame& dstFrame )
{
	const VideoFrame& src = static_cast<const VideoFrame&>( srcFrame );
	VideoFrame& dst = static_cast<VideoFrame&>( dstFrame );

	assert( src.desc().getWidth()  != 0 );
	assert( src.desc().getHeight() != 0 );
	assert( src.desc().getWidth()  == dst.desc().getWidth()  );
	assert( src.desc().getHeight() == dst.desc().getHeight() );
	assert( src.desc().getPixelDesc().getComponents() != 0 );
	assert( src.desc().getPixelDesc().getComponents() == dst.desc().getPixelDesc().getComponents() );

	if( ! _isInit )
		_isInit = init( srcFrame, dstFrame );

	for( size_t plane = 0; plane < MAX_SWS_PLANE; ++plane )
	{
		_srcData.at( plane ) = (uint8_t*) src.getPtr() + _srcOffsets.at( plane );
		_dstData.at( plane ) = (uint8_t*) dst.getPtr() + _dstOffsets.at( plane );
	}
	
	if( ! _imageConvertContext )
	{
		throw std::runtime_error( "unknown color convert context" );
	}

	if( _verbose )
	{
		std::clog << "source, offset:" << std::endl;
		std::clog << "[0] = " << &_srcOffsets[0] << std::endl;
		std::clog << "[1] = " << &_srcOffsets[1] << std::endl;
		std::clog << "[2] = " << &_srcOffsets[2] << std::endl;
		std::clog << "[3] = " << &_srcOffsets[3] << std::endl;
		
		std::clog << "source, slice:" << std::endl;
		std::clog << "[0] = " << &_srcData[0] << std::endl;
		std::clog << "[1] = " << &_srcData[1] << std::endl;
		std::clog << "[2] = " << &_srcData[2] << std::endl;
		std::clog << "[3] = " << &_srcData[3] << std::endl;
		
		std::clog << "destination, offset:" << std::endl;
		std::clog << "[0] = " << &_dstOffsets[0] << std::endl;
		std::clog << "[1] = " << &_dstOffsets[1] << std::endl;
		std::clog << "[2] = " << &_dstOffsets[2] << std::endl;
		std::clog << "[3] = " << &_dstOffsets[3] << std::endl;
		
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
