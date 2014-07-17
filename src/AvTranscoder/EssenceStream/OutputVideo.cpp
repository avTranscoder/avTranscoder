#include "OutputVideo.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
}

#include <AvTranscoder/DatasStructures/Image.hpp>
#include <AvTranscoder/Profile.hpp>

#include <stdexcept>
#include <cstdlib>

namespace avtranscoder
{

OutputVideo::OutputVideo( )
	: OutputEssence( )
	, _videoDesc( "mpeg2video" )
{
}

void OutputVideo::setup( )
{
	av_register_all();  // Warning: should be called only once

	AVCodecContext* codecContext( _videoDesc.getCodecContext() );

	if( codecContext == NULL )
	{
		throw std::runtime_error( "could not allocate video codec context" );
	}

	// try to open encoder with parameters
	int ret = avcodec_open2( codecContext, _videoDesc.getCodec(), NULL );
	if( ret < 0 )
	{
		char err[250];
		av_strerror( ret, err, 250);
		std::string msg = "could not open video encoder: ";
		msg += err;
		throw std::runtime_error( msg );
	}
}


bool OutputVideo::encodeFrame( const Frame& sourceFrame, DataStream& codedFrame )
{
#if LIBAVCODEC_VERSION_MAJOR > 54
	AVFrame* frame = av_frame_alloc();
#else
	AVFrame* frame = avcodec_alloc_frame();
#endif

	AVCodecContext* codecContext = _videoDesc.getCodecContext();

	// Set default frame parameters
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_unref( frame );
#else
	avcodec_get_frame_defaults( frame );
#endif

	const Image& sourceImageFrame = static_cast<const Image&>( sourceFrame );

	frame->width  = codecContext->width;
	frame->height = codecContext->height;
	frame->format = codecContext->pix_fmt;
	avpicture_fill( (AVPicture*)frame, const_cast< unsigned char * >( sourceImageFrame.getPtr() ), codecContext->pix_fmt, codecContext->width, codecContext->height );

	AVPacket packet;
	av_init_packet( &packet );
	// avcodec_encode_video allocate packet
	packet.size = 0;
	packet.data = NULL;
	packet.stream_index = 0;

	if( ( codecContext->coded_frame ) &&
		( codecContext->coded_frame->pts != (int)AV_NOPTS_VALUE ) )
	{
		// why need to do that ?
		//packet.pts = av_rescale_q( codecContext->coded_frame->pts, codecContext->time_base, codecContext->time_base );

		//std::cout << "pts with rescale " << (int)packet.pts << std::endl;
		packet.pts = codecContext->coded_frame->pts;
		//std::cout << "pts without rescale " << (int)packet.pts << std::endl;
	}

	if( codecContext->coded_frame &&
		codecContext->coded_frame->key_frame )
	{
		packet.flags |= AV_PKT_FLAG_KEY;
	}


#if LIBAVCODEC_VERSION_MAJOR > 53
	int gotPacket = 0;
	int ret = avcodec_encode_video2( codecContext, &packet, frame, &gotPacket );
	if( ret == 0 && gotPacket == 1 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
#else
	int ret = avcodec_encode_video( codecContext, packet.data, packet.size, frame );
	if( ret > 0 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
#endif
/*
		std::string imgType = "";
		switch( codecContext->coded_frame->pict_type )
		{
			case AV_PICTURE_TYPE_NONE : imgType = "None"; break;
			case AV_PICTURE_TYPE_I : imgType = "I"; break;
			case AV_PICTURE_TYPE_P : imgType = "P"; break;
			case AV_PICTURE_TYPE_B : imgType = "B"; break;
			case AV_PICTURE_TYPE_S : imgType = "S"; break;
			case AV_PICTURE_TYPE_SI : imgType = "SI"; break;
			case AV_PICTURE_TYPE_SP : imgType = "SP"; break;
			case AV_PICTURE_TYPE_BI : imgType = "BI"; break;
		}

		std::clog << "\tframe " << codecContext->coded_frame->display_picture_number;
		std::clog << " coded @ " << codecContext->coded_frame->coded_picture_number;
		std::clog << " type : " << imgType;
		std::clog << " quality : " << codecContext->coded_frame->quality << std::endl;
*/
	av_free_packet( &packet );
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_free( &frame );
	return ret == 0 && gotPacket == 1;
#else
 #if LIBAVCODEC_VERSION_MAJOR > 53
	avcodec_free_frame( &frame );
	return ret == 0 && gotPacket == 1;
 #else
	av_free( frame );
 #endif
#endif
	return ret == 0;
}


bool OutputVideo::encodeFrame( DataStream& codedFrame )
{
	AVCodecContext* codecContext = _videoDesc.getCodecContext();

	AVPacket packet;
	av_init_packet( &packet );
	// avcodec_encode_video allocate packet
	packet.size = 0;
	packet.data = NULL;
	packet.stream_index = 0;

#if LIBAVCODEC_VERSION_MAJOR > 53
	int gotPacket = 0;
	int ret = avcodec_encode_video2( codecContext, &packet, NULL, &gotPacket );
	if( ret == 0 && gotPacket == 1 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
	av_free_packet( &packet );
	return ret == 0 && gotPacket == 1;

#else
	int ret = avcodec_encode_video( codecContext, packet.data, packet.size, NULL );
	if( ret > 0 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
	av_free_packet( &packet );
	return ret == 0;

#endif
}

void OutputVideo::setProfile( Profile::ProfileDesc& desc, const avtranscoder::ImageDesc& imageDesc )
{
	if( ! desc.count( Profile::avProfileCodec ) ||
		! desc.count( Profile::avProfilePixelFormat ) || 
		! desc.count( Profile::avProfileFrameRate ) )
	{
		throw std::runtime_error( "The profile " + desc[ Profile::avProfileIdentificatorHuman ] + " is invalid." );
	}
	
	if( ( desc.count( Profile::avProfileWidth ) && std::strtoul( desc[ Profile::avProfileWidth ].c_str(), NULL, 0 ) != imageDesc.getWidth() ) || 
		( desc.count( Profile::avProfileHeight ) && std::strtoul( desc[ Profile::avProfileHeight ].c_str(), NULL, 0 ) != imageDesc.getHeight() ) )
	{
		throw std::runtime_error( "Invalid imageDesc with the profile " + desc[ Profile::avProfileIdentificatorHuman ] + "." );
	}
	
	_videoDesc.setVideoCodec( desc[ Profile::avProfileCodec ] );
	const size_t frameRate = std::strtoul( desc[ Profile::avProfileFrameRate ].c_str(), NULL, 0 );
	_videoDesc.setTimeBase( 1, frameRate );
	_videoDesc.setImageParameters( imageDesc );

	for( Profile::ProfileDesc::iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == Profile::avProfileIdentificator )
			continue;
		if( (*it).first == Profile::avProfileIdentificatorHuman )
			continue;
		if( (*it).first == Profile::avProfileType )
			continue;
		if( (*it).first == Profile::avProfileCodec )
			continue;
		if( (*it).first == Profile::avProfilePixelFormat )
			continue;
		if( (*it).first == Profile::avProfileWidth )
			continue;
		if( (*it).first == Profile::avProfileHeight )
			continue;
		if( (*it).first == Profile::avProfileFrameRate )
			continue;

		try
		{
			_videoDesc.set( (*it).first, (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "warning: " << e.what() << std::endl;
		}
	}

	setup();

	for( Profile::ProfileDesc::iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == Profile::avProfileIdentificator )
			continue;
		if( (*it).first == Profile::avProfileIdentificatorHuman )
			continue;
		if( (*it).first == Profile::avProfileType )
			continue;
		if( (*it).first == Profile::avProfileCodec )
			continue;
		if( (*it).first == Profile::avProfilePixelFormat )
			continue;
		if( (*it).first == Profile::avProfileWidth )
			continue;
		if( (*it).first == Profile::avProfileHeight )
			continue;
		if( (*it).first == Profile::avProfileFrameRate )
			continue;

		try
		{
			_videoDesc.set( (*it).first, (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "2.warning: " << e.what() << std::endl;
		}
	}
}

}