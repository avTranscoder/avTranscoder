#include "InputVideo.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

#include <iostream>
#include <stdexcept>

namespace avtranscoder
{

InputVideo::InputVideo( AvInputStream& inputStream )
	: InputEssence()
	, _inputStream   ( &inputStream )
	, _codec         ( NULL )
	, _codecContext  ( NULL )
	, _frame         ( NULL )
	, _selectedStream( -1 )
{
}

InputVideo::~InputVideo()
{
	if( _codecContext != NULL )
	{
		//avcodec_close( _codecContext );
		av_free( _codecContext );
		_codecContext = NULL;
	}
	if( _frame != NULL )
	{
#if LIBAVCODEC_VERSION_MAJOR > 54
		av_frame_free( &_frame );
#else
 #if LIBAVCODEC_VERSION_MAJOR > 53
		avcodec_free_frame( &_frame );
 #else
		av_free( _frame );
 #endif
#endif
		_frame = NULL;
	}
}

void InputVideo::setup()
{
	av_register_all();

	_codec = avcodec_find_decoder( _inputStream->getVideoDesc().getVideoCodecId() );
	if( _codec == NULL )
	{
		throw std::runtime_error( "codec not supported" );
	}

	_codecContext = avcodec_alloc_context3( _codec );
	if( _codecContext == NULL )
	{
		throw std::runtime_error( "unable to find context for codec" );
	}

	// if( codec->capabilities & CODEC_CAP_TRUNCATED )
	// 	codecContext->flags |= CODEC_FLAG_TRUNCATED;

	int ret = avcodec_open2( _codecContext, _codec, NULL );

	if( ret < 0 || _codecContext == NULL || _codec == NULL )
	{
		std::string msg = "unable open video codec: ";
		msg +=  _codec->long_name;
		msg += " (";
		msg += _codec->name;
		msg += ")";
		avcodec_close( _codecContext );
		throw std::runtime_error( msg );
	}

#if LIBAVCODEC_VERSION_MAJOR > 54
	_frame = av_frame_alloc();
#else
	_frame = avcodec_alloc_frame();
#endif
	if( _frame == NULL )
	{
		throw std::runtime_error( "unable to setup frame buffer" );
	}
}

bool InputVideo::readNextFrame( Frame& frameBuffer )
{
	int got_frame = 0;

	while( ! got_frame )
	{
		DataStream data;
		if( ! _inputStream->readNextPacket( data ) )
			return false;

		AVPacket packet;
		av_init_packet( &packet );

		packet.stream_index = _selectedStream;
		packet.data         = data.getPtr();
		packet.size         = data.getSize();
		
		int ret = avcodec_decode_video2( _codecContext, _frame, &got_frame, &packet );
		
		if( ret < 0 )
		{
			char err[250];
			av_strerror( ret, err, 250);
			
			throw std::runtime_error( "an error occured during video decoding - " + std::string(err) );
		}
		
		av_free_packet( &packet );
	}

	Image& imageBuffer = static_cast<Image&>( frameBuffer );

	size_t decodedSize = avpicture_get_size( (AVPixelFormat)_frame->format, _frame->width, _frame->height );
	if( imageBuffer.getBuffer().size() != decodedSize )
		imageBuffer.getBuffer().resize( decodedSize );

	// Copy pixel data from an AVPicture into one contiguous buffer.
	avpicture_layout( (AVPicture*)_frame, (AVPixelFormat)_frame->format, _frame->width, _frame->height, &imageBuffer.getBuffer()[0], frameBuffer.getBuffer().size() );

	return true;
}

bool InputVideo::readNextFrame( std::vector<Frame>& frameBuffer )
{

}

void InputVideo::flushDecoder()
{
	avcodec_flush_buffers( _codecContext );
}

}
