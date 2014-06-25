#include "ColorTransform.hpp"
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

ColorTransform::ColorTransform()
	: EssenceTransform()
	, _imageConvertContext( NULL )
	, _srcData     ( (uint8_t)MAX_SWS_PLANE, NULL )
	, _dstData     ( (uint8_t)MAX_SWS_PLANE, NULL )
	, _srcLineSize ( MAX_SWS_PLANE, 0 )
	, _dstLineSize ( MAX_SWS_PLANE, 0 )
	, _srcOffsets  ( MAX_SWS_PLANE, 0 )
	, _dstOffsets  ( MAX_SWS_PLANE, 0 )
	, _isInit      ( false )
{
}

bool ColorTransform::init( const Frame& src, const Frame& dst )
{
	const Image& srcImage = static_cast<const Image&>( src );
	const Image& dstImage = static_cast<const Image&>( dst );

	assert( srcImage.desc().getWidth()  != 0 );
	assert( srcImage.desc().getHeight() != 0 );
	assert( srcImage.desc().getWidth()  == dstImage.desc().getWidth()  );
	assert( srcImage.desc().getHeight() == dstImage.desc().getHeight() );

	_imageConvertContext = sws_getContext(
		srcImage.desc().getWidth(), srcImage.desc().getHeight(), srcImage.desc().getPixelDesc().findPixel(),
		dstImage.desc().getWidth(), dstImage.desc().getHeight(), dstImage.desc().getPixelDesc().findPixel(),
		SWS_POINT, NULL, NULL, NULL);

	if( ! _imageConvertContext )
	{
		throw std::runtime_error( "unable to create color convert context" );
	}

	av_image_fill_linesizes( &_srcLineSize[0], srcImage.desc().getPixelDesc().findPixel(), srcImage.desc().getWidth() );
	av_image_fill_linesizes( &_dstLineSize[0], dstImage.desc().getPixelDesc().findPixel(), dstImage.desc().getWidth() );

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

		cumulSrcOffset += _srcLineSize.at( plane ) * srcImage.desc().getHeight();
		cumulDstOffset += _dstLineSize.at( plane ) * dstImage.desc().getHeight();
	}

	return true;
}

void ColorTransform::convert( const Frame& src, Frame& dst )
{
	const Image& srcImage = static_cast<const Image&>( src );
	Image& dstImage = static_cast<Image&>( dst );

	assert( srcImage.desc().getWidth()  != 0 );
	assert( srcImage.desc().getHeight() != 0 );
	assert( srcImage.desc().getWidth()  == dstImage.desc().getWidth()  );
	assert( srcImage.desc().getHeight() == dstImage.desc().getHeight() );
	assert( srcImage.desc().getPixelDesc().getComponents() != 0 );
	assert( srcImage.desc().getPixelDesc().getComponents() == dstImage.desc().getPixelDesc().getComponents() );

	if( ! _isInit )
		_isInit = init( srcImage, dstImage );

	for( size_t plane = 0; plane < MAX_SWS_PLANE; ++plane )
	{
		_srcData.at( plane ) = (uint8_t*)const_cast< unsigned char* >( srcImage.getPtr() + _srcOffsets.at( plane ) );
		_dstData.at( plane ) = (uint8_t*)dstImage.getPtr() + _dstOffsets.at( plane );
	}
	
	if( !_imageConvertContext )
	{
		throw std::runtime_error( "unknown color convert context" );
	}

	int ret = sws_scale( _imageConvertContext,
		&_srcData[0], &_srcLineSize[0], 0, srcImage.desc().getHeight(),
		&_dstData[0], &_dstLineSize[0] );

	if( ret != (int) srcImage.desc().getHeight() )
		throw std::runtime_error( "error in color converter" );
}

}
