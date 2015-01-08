#include "AvOutputVideo.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <iostream>
#include <stdexcept>
#include <cstdlib>

namespace avtranscoder
{

AvOutputVideo::AvOutputVideo( const std::string& videoCodecName )
	: _codec( eCodecTypeEncoder, videoCodecName )
{
}

void AvOutputVideo::setup( )
{
	av_register_all();

	AVCodecContext& avCodecContext( _codec.getAVCodecContext() );

	if( &avCodecContext == NULL )
	{
		throw std::runtime_error( "could not allocate video codec context" );
	}

	// try to open encoder with parameters
	int ret = avcodec_open2( &avCodecContext, &_codec.getAVCodec(), NULL );
	if( ret < 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( ret, err, sizeof(err) );
		std::string msg = "could not open video encoder " + _codec.getCodecName() +": ";
		msg += err;
		throw std::runtime_error( msg );
	}
}


bool AvOutputVideo::encodeFrame( const Frame& sourceFrame, Frame& codedFrame )
{
#if LIBAVCODEC_VERSION_MAJOR > 54
	AVFrame* frame = av_frame_alloc();
#else
	AVFrame* frame = avcodec_alloc_frame();
#endif

	AVCodecContext& avCodecContext = _codec.getAVCodecContext();

	// Set default frame parameters
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_unref( frame );
#else
	avcodec_get_frame_defaults( frame );
#endif

	const VideoFrame& sourceImageFrame = static_cast<const VideoFrame&>( sourceFrame );

	frame->width  = avCodecContext.width;
	frame->height = avCodecContext.height;
	frame->format = avCodecContext.pix_fmt;
	avpicture_fill( (AVPicture*)frame, const_cast< unsigned char * >( sourceImageFrame.getPtr() ), avCodecContext.pix_fmt, avCodecContext.width, avCodecContext.height );

	AVPacket packet;
	av_init_packet( &packet );
	// avcodec_encode_video allocate packet
	packet.size = 0;
	packet.data = NULL;
	packet.stream_index = 0;

	if( ( avCodecContext.coded_frame ) &&
		( avCodecContext.coded_frame->pts != (int)AV_NOPTS_VALUE ) )
	{
		packet.pts = avCodecContext.coded_frame->pts;
	}

	if( avCodecContext.coded_frame &&
		avCodecContext.coded_frame->key_frame )
	{
		packet.flags |= AV_PKT_FLAG_KEY;
	}

#if LIBAVCODEC_VERSION_MAJOR > 53
	int gotPacket = 0;
	int ret = avcodec_encode_video2( &avCodecContext, &packet, frame, &gotPacket );
	if( ret == 0 && gotPacket == 1 )
	{
		codedFrame.copyData( packet.data, packet.size );
	}
#else
	int ret = avcodec_encode_video( &avCodecContext, packet.data, packet.size, frame );
	if( ret > 0 )
	{
		codedFrame.copyData( packet.data, packet.size );
	}
#endif

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

bool AvOutputVideo::encodeFrame( Frame& codedFrame )
{
	AVCodecContext& avCodecContext = _codec.getAVCodecContext();

	AVPacket packet;
	av_init_packet( &packet );
	// avcodec_encode_video allocate packet
	packet.size = 0;
	packet.data = NULL;
	packet.stream_index = 0;

#if LIBAVCODEC_VERSION_MAJOR > 53
	int gotPacket = 0;
	int ret = avcodec_encode_video2( &avCodecContext, &packet, NULL, &gotPacket );
	if( ret == 0 && gotPacket == 1 )
	{
		codedFrame.copyData( packet.data, packet.size );
	}
	av_free_packet( &packet );
	return ret == 0 && gotPacket == 1;
#else
	int ret = avcodec_encode_video( &avCodecContext, packet.data, packet.size, NULL );
	if( ret > 0 )
	{
		codedFrame.copyData( packet.data, packet.size );
	}
	av_free_packet( &packet );
	return ret == 0;
#endif
}

void AvOutputVideo::setProfile( const ProfileLoader::Profile& profile, const avtranscoder::VideoFrameDesc& frameDesc )
{
	if( ! profile.count( constants::avProfileCodec ) ||
		! profile.count( constants::avProfileFrameRate ) )
	{
		throw std::runtime_error( "The profile " + profile.find( constants::avProfileIdentificatorHuman )->second + " is invalid." );
	}
	
	_codec.setCodec( eCodecTypeEncoder, profile.find( constants::avProfileCodec )->second );

	const size_t frameRate = std::strtoul( profile.find( constants::avProfileFrameRate )->second.c_str(), NULL, 0 );
	_codec.setTimeBase( 1, frameRate );

	_codec.setImageParameters( frameDesc );
	
	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType ||
			(*it).first == constants::avProfileCodec ||
			(*it).first == constants::avProfileFrameRate )
			continue;

		try
		{
			Option& encodeOption = _codec.getCodecContext().getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{}
	}

	setup();

	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType ||
			(*it).first == constants::avProfileCodec ||
			(*it).first == constants::avProfileFrameRate )
			continue;

		try
		{
			Option& encodeOption = _codec.getCodecContext().getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[OutputVideo] warning - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what() << std::endl;
		}
	}
}

}