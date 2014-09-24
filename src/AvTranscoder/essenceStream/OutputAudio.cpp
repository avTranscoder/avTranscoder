#include "OutputAudio.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <iostream>
#include <stdexcept>

namespace avtranscoder
{

OutputAudio::OutputAudio()
	: IOutputEssence( "pcm_s16le" )
{
}

void OutputAudio::setup()
{
	av_register_all();  // Warning: should be called only once

	AVCodecContext* codecContext( _codedDesc.getCodecContext() );

	if( codecContext == NULL )
	{
		throw std::runtime_error( "could not allocate audio codec context" );
	}
	
	// try to open encoder with parameters.
	int ret = avcodec_open2( codecContext, _codedDesc.getCodec(), NULL );
	if( ret < 0 )
	{
		char err[250];
		av_strerror( ret, err, 250);
		std::string msg = "could not open audio encoder: ";
		msg += err;
		throw std::runtime_error( msg );
	}
}

bool OutputAudio::encodeFrame( const Frame& sourceFrame, DataStream& codedFrame )
{
#if LIBAVCODEC_VERSION_MAJOR > 54
	AVFrame* frame = av_frame_alloc();
#else
	AVFrame* frame = avcodec_alloc_frame();
#endif

	AVCodecContext* codecContext = _codedDesc.getCodecContext();

	// Set default frame parameters
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_unref( frame );
#else
	avcodec_get_frame_defaults( frame );
#endif

	const AudioFrame& sourceAudioFrame = static_cast<const AudioFrame&>( sourceFrame );
	
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

bool OutputAudio::encodeFrame( DataStream& codedFrame )
{
	AVCodecContext* codecContext = _codedDesc.getCodecContext();

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

void OutputAudio::setProfile( const Profile::ProfileDesc& desc, const AudioFrameDesc& frameDesc  )
{
	if( ! desc.count( Profile::avProfileCodec ) || 		
		! desc.count( Profile::avProfileSampleFormat ) )
	{
		throw std::runtime_error( "The profile " + desc.find( Profile::avProfileIdentificatorHuman )->second + " is invalid." );
	}
	
	_codedDesc.setCodec( desc.find( Profile::avProfileCodec )->second );
	
	static_cast<AudioDesc>( _codedDesc ).setAudioParameters( frameDesc );

	ParamSet paramSet( _codedDesc.getCodecContext() );
	
	for( Profile::ProfileDesc::const_iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == Profile::avProfileIdentificator ||
			(*it).first == Profile::avProfileIdentificatorHuman ||
			(*it).first == Profile::avProfileType ||
			(*it).first == Profile::avProfileCodec ||
			(*it).first == Profile::avProfileSampleFormat )
			continue;

		try
		{
			paramSet.set( (*it).first, (*it).second );
		}
		catch( std::exception& e )
		{
			//std::cout << "[OutputAudio] warning: " << e.what() << std::endl;
		}
	}

	setup();

	for( Profile::ProfileDesc::const_iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == Profile::avProfileIdentificator ||
			(*it).first == Profile::avProfileIdentificatorHuman ||
			(*it).first == Profile::avProfileType ||
			(*it).first == Profile::avProfileCodec ||
			(*it).first == Profile::avProfileSampleFormat )
			continue;

		try
		{
			paramSet.set( (*it).first, (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[OutputAudio] warning: " << e.what() << std::endl;
		}
	}
}

}

