#include "VideoEssenceTransform.hpp"
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

#include <AvTranscoder/DatasStructures/Pixel.hpp>
#include <AvTranscoder/DatasStructures/Image.hpp>

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
{
}

bool VideoEssenceTransform::init( const Frame& srcFrame, const Frame& dstFrame )
{
	const Image& src = static_cast<const Image&>( srcFrame );
	const Image& dst = static_cast<const Image&>( dstFrame );

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
	const Image& src = static_cast<const Image&>( srcFrame );
	Image& dst = static_cast<Image&>( dstFrame );

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
		_srcData.at( plane ) = (uint8_t*) const_cast< unsigned char* >( src.getPtr() + _srcOffsets.at( plane ) );
		_dstData.at( plane ) = (uint8_t*) dst.getPtr() + _dstOffsets.at( plane );
	}
	
	if( !_imageConvertContext )
	{
		throw std::runtime_error( "unknown color convert context" );
	}

	int ret = sws_scale( _imageConvertContext,
		&_srcData[0], &_srcLineSize[0], 0, src.desc().getHeight(),
		&_dstData[0], &_dstLineSize[0] );

	if( ret != (int) src.desc().getHeight() )
		throw std::runtime_error( "error in color converter" );
}

}
