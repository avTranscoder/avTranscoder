#include "ColorTransform.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libavutil/frame.h>
}

#include <iostream>
#include <iomanip>

namespace avtranscoder
{

ColorTransform::ColorTransform()
	: m_width( 0 )
	, m_height( 0 )
{
	// m_inputPixel  = AV_PIX_FMT_YUV422P10;
	// m_outputPixel = AV_PIX_FMT_YUV422P;
}

void ColorTransform::setWidth ( const size_t width )
{
	m_width = width;
}

void ColorTransform::setHeight( const size_t height )
{
	m_height = height;
}

void ColorTransform::setInputPixel( const Pixel& pixel )
{
	m_inputPixel = pixel;
}

void ColorTransform::setOutputPixel( const Pixel& pixel )
{
	m_outputPixel = pixel;
}

bool ColorTransform::init()
{
	return true;
}

void ColorTransform::convert( const Image& src, Image& dst )
{

	SwsContext* m_imageConvertContext = sws_getContext(
		m_width, m_height, m_inputPixel(),
		m_width, m_height, m_outputPixel(),
		SWS_POINT, NULL, NULL, NULL);

	// resize dst buffer ? using :
	// av_image_get_buffer_size(enum AVPixelFormat pix_fmt, int width, int height, int align);

	unsigned char* dataIn     [ AV_NUM_DATA_POINTERS ];
	unsigned char* dataOut    [ AV_NUM_DATA_POINTERS ];
	int            linesizeIn [ AV_NUM_DATA_POINTERS ];
	int            linesizeOut[ AV_NUM_DATA_POINTERS ];

	// std::vector< int > lineSizeOut( AV_NUM_DATA_POINTERS, 0 );

	for( size_t i = 0; i < AV_NUM_DATA_POINTERS; ++i )
	{
		dataIn [i] = NULL;
		dataOut[i] = NULL;
		linesizeIn [i] = 0;
		linesizeOut[i] = 0;
	}

	//dataIn [0] = const_cast< unsigned char* >( &src[0] );
	//dataOut[0] = &dst[0];
	linesizeIn [0] = av_image_get_linesize( m_inputPixel(),  m_width, 3 );
	linesizeOut[0] = av_image_get_linesize( m_outputPixel(), m_width, 3 );

	sws_scale( m_imageConvertContext,
		dataIn, linesizeIn, 0, m_height,
		dataOut, linesizeOut );
}

}