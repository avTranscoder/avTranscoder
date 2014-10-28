#include "AvOutputAudio.hpp"

#include <mediaCore/option/Context.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <iostream>
#include <stdexcept>

namespace mediamanager
{
namespace mediaio
{

AvOutputAudio::AvOutputAudio()
	: _codec( eCodecTypeEncoder, "pcm_s16le" )
{
}

void AvOutputAudio::setup()
{
	av_register_all();

	AVCodecContext* codecContext( _codec.getAVCodecContext() );

	if( codecContext == NULL )
	{
		throw std::runtime_error( "could not allocate audio codec context" );
	}
	
	// try to open encoder with parameters.
	int ret = avcodec_open2( codecContext, _codec.getAVCodec(), NULL );
	if( ret < 0 )
	{
		char err[250];
		av_strerror( ret, err, 250);
		std::string msg = "could not open audio encoder: ";
		msg += err;
		throw std::runtime_error( msg );
	}
}

bool AvOutputAudio::encodeFrame( const mediacore::Frame& sourceFrame, mediacore::Frame& codedFrame )
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

	const mediacore::AudioFrame& sourceAudioFrame = static_cast<const mediacore::AudioFrame&>( sourceFrame );
	
	frame->nb_samples     = sourceAudioFrame.getNbSamples();
	frame->format         = codecContext->sample_fmt;
	frame->channel_layout = codecContext->channel_layout;
	
	// we calculate the size of the samples buffer in bytes
	int buffer_size = av_samples_get_buffer_size( NULL, codecContext->channels, frame->nb_samples, codecContext->sample_fmt, 0 );
	if( buffer_size < 0 )
	{
		char err[250];
		av_strerror( buffer_size, err, 250 );
		
		throw std::runtime_error( "EncodeFrame error: buffer size < 0 - " + std::string(err) );
	}

	int retvalue = avcodec_fill_audio_frame( frame, codecContext->channels, codecContext->sample_fmt, sourceAudioFrame.getPtr(), buffer_size, 0 );
	if( retvalue < 0 )
	{
		char err[250];
		av_strerror( retvalue, err, 250);	
		
		throw std::runtime_error( "EncodeFrame error: avcodec fill audio frame - " + std::string( err ) );
	}
	
	AVPacket packet;
	av_init_packet( &packet );
	
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
	int ret = avcodec_encode_audio2( codecContext, &packet, frame, &gotPacket );
	if( ret == 0 && gotPacket == 1 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
#else
	int ret = avcodec_encode_audio( codecContext, packet.data, packet.size, frame );
	if( ret > 0 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
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

bool AvOutputAudio::encodeFrame( mediacore::Frame& codedFrame )
{
	AVCodecContext* codecContext = _codec.getAVCodecContext();

	AVPacket packet;
	av_init_packet( &packet );
	
	packet.size = 0;
	packet.data = NULL;
	packet.stream_index = 0;

#if LIBAVCODEC_VERSION_MAJOR > 53
	int gotPacket = 0;
	int ret = avcodec_encode_audio2( codecContext, &packet, NULL, &gotPacket );
	if( ret == 0 && gotPacket == 1 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
	av_free_packet( &packet );
	return ret == 0 && gotPacket == 1;

#else
	int ret = avcodec_encode_audio( codecContext, packet.data, packet.size, NULL );
	if( ret > 0 )
	{
		codedFrame.getBuffer().resize( packet.size );
		memcpy( codedFrame.getPtr(), packet.data, packet.size );
	}
	av_free_packet( &packet );
	return ret == 0;

#endif
}

void AvOutputAudio::setProfile( const mediacore::Profile::ProfileDesc& desc, const mediacore::AudioFrameDesc& frameDesc  )
{
	using namespace mediacore::constants;
	if( ! desc.count( mediaProfileCodec ) ||
		! desc.count( mediaProfileSampleFormat ) )
	{
		throw std::runtime_error( "The profile " + desc.find( mediaProfileIdentificatorHuman )->second + " is invalid." );
	}
	
	_codec.setCodec( eCodecTypeEncoder, desc.find( mediaProfileCodec )->second );
	_codec.setAudioParameters( frameDesc );

	mediacore::Context codecContext( _codec.getAVCodecContext() );

	for( mediacore::Profile::ProfileDesc::const_iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == mediaProfileIdentificator ||
			(*it).first == mediaProfileIdentificatorHuman ||
			(*it).first == mediaProfileType ||
			(*it).first == mediaProfileCodec ||
			(*it).first == mediaProfileSampleFormat )
			continue;

		try
		{
			mediacore::Option& encodeOption = codecContext.getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			//std::cout << "[OutputAudio] warning: " << e.what() << std::endl;
		}
	}

	setup();

	for( mediacore::Profile::ProfileDesc::const_iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == mediaProfileIdentificator ||
			(*it).first == mediaProfileIdentificatorHuman ||
			(*it).first == mediaProfileType ||
			(*it).first == mediaProfileCodec ||
			(*it).first == mediaProfileSampleFormat )
			continue;

		try
		{
			mediacore::Option& encodeOption = codecContext.getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[OutputAudio] warning: " << e.what() << std::endl;
		}
	}
}

}
}
