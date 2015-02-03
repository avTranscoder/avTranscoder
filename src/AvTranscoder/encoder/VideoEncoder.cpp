#include "VideoEncoder.hpp"

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

VideoEncoder::VideoEncoder( const std::string& videoCodecName )
	: _codec( eCodecTypeEncoder, videoCodecName )
{
}

void VideoEncoder::setup()
{
	_codec.open();
}


bool VideoEncoder::encodeFrame( const Frame& sourceFrame, Frame& codedFrame )
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

	AVPacket& packet = codedFrame.getAVPacket();
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
	if( ret != 0 && gotPacket == 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( ret, err, sizeof(err) );
		throw std::runtime_error( "EncodeFrame error: avcodec encode video frame - " + std::string( err ) );
	}
#else
	int ret = avcodec_encode_video( &avCodecContext, packet.data, packet.size, frame );
	if( ret < 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( ret, err, sizeof(err) );
		throw std::runtime_error( "EncodeFrame error: avcodec encode video frame - " + std::string( err ) );
	}
#endif

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

bool VideoEncoder::encodeFrame( Frame& codedFrame )
{
	AVCodecContext& avCodecContext = _codec.getAVCodecContext();

	AVPacket& packet = codedFrame.getAVPacket();
	packet.stream_index = 0;

#if LIBAVCODEC_VERSION_MAJOR > 53
	int gotPacket = 0;
	int ret = avcodec_encode_video2( &avCodecContext, &packet, NULL, &gotPacket );
	if( ret != 0 && gotPacket == 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( ret, err, sizeof(err) );
		throw std::runtime_error( "EncodeFrame error: avcodec encode last video frame - " + std::string( err ) );
	}
	return ret == 0 && gotPacket == 1;
#else
	int ret = avcodec_encode_video( &avCodecContext, packet.data, packet.size, NULL );
	if( ret < 0 )
	{
		char err[AV_ERROR_MAX_STRING_SIZE];
		av_strerror( ret, err, sizeof(err) );
		throw std::runtime_error( "EncodeFrame error: avcodec encode last video frame - " + std::string( err ) );
	}
	return ret == 0;
#endif
}

void VideoEncoder::setProfile( const ProfileLoader::Profile& profile, const avtranscoder::VideoFrameDesc& frameDesc )
{
	// set width, height, pixel format, fps
	_codec.setImageParameters( frameDesc );

	// set encoder options
	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType ||
			(*it).first == constants::avProfileCodec ||
			(*it).first == constants::avProfileWidth ||
			(*it).first == constants::avProfileHeight ||
			(*it).first == constants::avProfilePixelFormat ||
			(*it).first == constants::avProfileFrameRate )
			continue;

		try
		{
			Option& encodeOption = _codec.getOption( (*it).first );
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
			(*it).first == constants::avProfileWidth ||
			(*it).first == constants::avProfileHeight ||
			(*it).first == constants::avProfilePixelFormat ||
			(*it).first == constants::avProfileFrameRate )
			continue;

		try
		{
			Option& encodeOption = _codec.getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[VideoEncoder] warning - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what() << std::endl;
		}
	}
}

}