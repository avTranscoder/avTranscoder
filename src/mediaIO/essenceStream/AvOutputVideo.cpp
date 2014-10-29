#include "AvOutputVideo.hpp"

#include <mediaCore/option/Context.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <iostream>
#include <stdexcept>
#include <cstdlib>

namespace mediaengine
{
namespace mediaio
{

AvOutputVideo::AvOutputVideo( )
	: _codec( eCodecTypeEncoder, "mpeg2video" )
{
}

void AvOutputVideo::setup( )
{
	av_register_all();

	AVCodecContext* codecContext( _codec.getAVCodecContext() );

	if( codecContext == NULL )
	{
		throw std::runtime_error( "could not allocate video codec context" );
	}

	// try to open encoder with parameters
	int ret = avcodec_open2( codecContext, _codec.getAVCodec(), NULL );
	if( ret < 0 )
	{
		char err[250];
		av_strerror( ret, err, 250);
		std::string msg = "could not open video encoder: ";
		msg += err;
		throw std::runtime_error( msg );
	}
}


bool AvOutputVideo::encodeFrame( const mediacore::Frame& sourceFrame, mediacore::Frame& codedFrame )
{
#if LIBAVCODEC_VERSION_MAJOR > 54
	AVFrame* frame = av_frame_alloc();
#else
	AVFrame* frame = avcodec_alloc_frame();
#endif

	AVCodecContext* codecContext = _codec.getAVCodecContext();

	// Set default frame parameters
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_unref( frame );
#else
	avcodec_get_frame_defaults( frame );
#endif

	const mediacore::VideoFrame& sourceImageFrame = static_cast<const mediacore::VideoFrame&>( sourceFrame );

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
		packet.pts = codecContext->coded_frame->pts;
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

bool AvOutputVideo::encodeFrame( mediacore::Frame& codedFrame )
{
	AVCodecContext* codecContext = _codec.getAVCodecContext();

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

void AvOutputVideo::setProfile( const mediacore::Profile::ProfileDesc& desc, const mediacore::VideoFrameDesc& frameDesc )
{
	using namespace mediacore::constants;
	if( ! desc.count( mediaProfileCodec ) ||
		! desc.count( mediaProfilePixelFormat ) || 
		! desc.count( mediaProfileFrameRate ) )
	{
		throw std::runtime_error( "The profile " + desc.find( mediaProfileIdentificatorHuman )->second + " is invalid." );
	}
	
	_codec.setCodec( eCodecTypeEncoder, desc.find( mediaProfileCodec )->second );

	const size_t frameRate = std::strtoul( desc.find( mediaProfileFrameRate )->second.c_str(), NULL, 0 );
	_codec.setTimeBase( 1, frameRate );

	_codec.setImageParameters( frameDesc );

	mediacore::Context codecContext( _codec.getAVCodecContext() );
	
	for( mediacore::Profile::ProfileDesc::const_iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == mediaProfileIdentificator ||
			(*it).first == mediaProfileIdentificatorHuman ||
			(*it).first == mediaProfileType ||
			(*it).first == mediaProfileCodec ||
			(*it).first == mediaProfilePixelFormat ||
			(*it).first == mediaProfileFrameRate )
			continue;

		try
		{
			mediacore::Option& encodeOption = codecContext.getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			//std::cout << "[OutputVideo] warning: " << e.what() << std::endl;
		}
	}

	setup();

	for( mediacore::Profile::ProfileDesc::const_iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == mediaProfileIdentificator ||
			(*it).first == mediaProfileIdentificatorHuman ||
			(*it).first == mediaProfileType ||
			(*it).first == mediaProfileCodec ||
			(*it).first == mediaProfilePixelFormat ||
			(*it).first == mediaProfileFrameRate )
			continue;

		try
		{
			mediacore::Option& encodeOption = codecContext.getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[OutputVideo] warning: " << e.what() << std::endl;
		}
	}
}

}
}
