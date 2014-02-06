#include "ColorTransform.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libavutil/frame.h>
}

#include <iostream>
#include <iomanip>
#include <assert.h>
#include <stdexcept>

namespace avtranscoder
{

ColorTransform::ColorTransform()
	: m_imageConvertContext( NULL )
	, srcData     ( MAX_SWS_PLANE, NULL )
	, dstData     ( MAX_SWS_PLANE, NULL )
	, srcLineSize ( MAX_SWS_PLANE, 0 )
	, dstLineSize ( MAX_SWS_PLANE, 0 )
	, srcOffsets  ( MAX_SWS_PLANE, 0 )
	, dstOffsets  ( MAX_SWS_PLANE, 0 )
{
}

bool ColorTransform::init( const Image& src, const Image& dst )
{
	assert( src.desc().getWidth()  != 0 );
	assert( src.desc().getHeight() != 0 );
	assert( src.desc().getWidth()  == dst.desc().getWidth()  );
	assert( src.desc().getHeight() == dst.desc().getHeight() );

	m_imageConvertContext = sws_getContext(
		src.desc().getWidth(), src.desc().getHeight(), src.desc().getPixelDesc().findPixel(),
		dst.desc().getWidth(), dst.desc().getHeight(), dst.desc().getPixelDesc().findPixel(),
		SWS_POINT, NULL, NULL, NULL);

	av_image_fill_linesizes( &srcLineSize[0], src.desc().getPixelDesc().findPixel(), src.desc().getWidth() );
	av_image_fill_linesizes( &dstLineSize[0], dst.desc().getPixelDesc().findPixel(), dst.desc().getWidth() );

	size_t cumulSrcOffset = 0;
	size_t cumulDstOffset = 0;

	for( size_t plane = 0; plane < MAX_SWS_PLANE; ++plane )
	{
		srcLineSize.at( plane ) = FFALIGN( srcLineSize.at( plane ), 16 );
		dstLineSize.at( plane ) = FFALIGN( dstLineSize.at( plane ), 16 );

		srcOffsets.at( plane ) = cumulSrcOffset;
		dstOffsets.at( plane ) = cumulDstOffset;

		cumulSrcOffset += srcLineSize.at( plane ) * src.desc().getHeight();
		cumulDstOffset += dstLineSize.at( plane ) * dst.desc().getHeight();
	}

	return true;
}

void ColorTransform::convert( const Image& src, Image& dst )
{
	assert( src.desc().getWidth()  != 0 );
	assert( src.desc().getHeight() != 0 );
	assert( src.desc().getWidth()  == dst.desc().getWidth()  );
	assert( src.desc().getHeight() == dst.desc().getHeight() );
	assert( src.desc().getPixelDesc().getComponents() != 0 );
	assert( src.desc().getPixelDesc().getComponents() == dst.desc().getPixelDesc().getComponents() );

	if( ! m_isInit )
		m_isInit = init( src, dst );

	for( size_t plane = 0; plane < MAX_SWS_PLANE; ++plane )
	{
		srcData.at( plane ) = (uint8_t*)const_cast< unsigned char* >( src.getPtr() + srcOffsets.at( plane ) );
		dstData.at( plane ) = (uint8_t*)dst.getPtr() + dstOffsets.at( plane );
	}
	
	if( !m_imageConvertContext )
	{
		throw std::runtime_error( "unknown convert context" );
	}

	int ret = sws_scale( m_imageConvertContext,
		&srcData[0], &srcLineSize[0], 0, src.desc().getHeight(),
		&dstData[0], &dstLineSize[0] );

	if( ret != src.desc().getHeight() )
		throw std::runtime_error( "error in color converter" );
}

}