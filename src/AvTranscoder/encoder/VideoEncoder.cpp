#include "VideoEncoder.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <stdexcept>
#include <cstdlib>

namespace avtranscoder
{

VideoEncoder::VideoEncoder( const std::string& videoCodecName )
	: _codec( eCodecTypeEncoder, videoCodecName )
	, _frame( NULL )
{
#if LIBAVCODEC_VERSION_MAJOR > 54
	_frame = av_frame_alloc();
#else
	_frame = avcodec_alloc_frame();
#endif
}

VideoEncoder::~VideoEncoder()
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
}

void VideoEncoder::setupVideoEncoder( const VideoFrameDesc& frameDesc, const ProfileLoader::Profile& profile )
{
	LOG_DEBUG( "Set profile of video encoder with:\n" << profile )

	// set width, height, pixel format, fps
	_codec.setImageParameters( frameDesc );

	// setup encoder
	setupEncoder( profile );
}

void VideoEncoder::setupEncoder( const ProfileLoader::Profile& profile )
{
	// set threads before any other options
	if( profile.count( constants::avProfileThreads ) )
		_codec.getOption( constants::avProfileThreads ).setString( profile.at( constants::avProfileThreads ) );
	else
		_codec.getOption( constants::avProfileThreads ).setString( "auto" );

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
			(*it).first == constants::avProfileFrameRate ||
			(*it).first == constants::avProfileThreads )
			continue;

		try
		{
			Option& encodeOption = _codec.getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{}
	}

	// open encoder
	_codec.openCodec();

	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType ||
			(*it).first == constants::avProfileCodec ||
			(*it).first == constants::avProfileWidth ||
			(*it).first == constants::avProfileHeight ||
			(*it).first == constants::avProfilePixelFormat ||
			(*it).first == constants::avProfileFrameRate ||
			(*it).first == constants::avProfileThreads  )
			continue;

		try
		{
			Option& encodeOption = _codec.getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			LOG_WARN( "VideoEncoder - can't set option " << (*it).first <<  " to " << (*it).second << ": " << e.what() )
		}
	}
}

bool VideoEncoder::encodeFrame( const Frame& sourceFrame, Frame& codedFrame )
{
	AVCodecContext& avCodecContext = _codec.getAVCodecContext();

	// Set default frame parameters
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_unref( _frame );
#else
	avcodec_get_frame_defaults( _frame );
#endif

	const VideoFrame& sourceImageFrame = static_cast<const VideoFrame&>( sourceFrame );

	_frame->width  = avCodecContext.width;
	_frame->height = avCodecContext.height;
	_frame->format = avCodecContext.pix_fmt;

	int bufferSize = avpicture_fill( (AVPicture*)_frame, const_cast< unsigned char * >( sourceImageFrame.getData() ), avCodecContext.pix_fmt, avCodecContext.width, avCodecContext.height );
	if( bufferSize < 0 )
	{
		throw std::runtime_error( "Encode video frame error: buffer size < 0 - " + getDescriptionFromErrorCode( bufferSize ) );
	}

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
	int ret = avcodec_encode_video2( &avCodecContext, &packet, _frame, &gotPacket );
	if( ret != 0 && gotPacket == 0 )
	{
		throw std::runtime_error( "Encode video frame error: avcodec encode video frame - " + getDescriptionFromErrorCode( ret ) );
	}
#else
	int ret = avcodec_encode_video( &avCodecContext, packet.data, packet.size, _frame );
	if( ret < 0 )
	{
		throw std::runtime_error( "Encode video frame error: avcodec encode video frame - " + getDescriptionFromErrorCode( ret ) );
	}
#endif

#if LIBAVCODEC_VERSION_MAJOR > 53
	return ret == 0 && gotPacket == 1;
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
		throw std::runtime_error( "Encode video frame error: avcodec encode last video frame - " + getDescriptionFromErrorCode( ret ) );
	}
	return ret == 0 && gotPacket == 1;
#else
	int ret = avcodec_encode_video( &avCodecContext, packet.data, packet.size, NULL );
	if( ret < 0 )
	{
		throw std::runtime_error( "Encode video frame error: avcodec encode last video frame - " + getDescriptionFromErrorCode( ret ) );
	}
	return ret == 0;
#endif
}

}