#include "OutputAudio.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <AvTranscoder/DatasStructures/AudioFrame.hpp>
#include <AvTranscoder/Profile.hpp>

#include <stdexcept>
#include <cstdlib>

namespace avtranscoder
{

OutputAudio::OutputAudio()
	: OutputEssence()
	, _audioDesc( "pcm_s16le" )
{
}

void OutputAudio::setup()
{
	av_register_all();  // Warning: should be called only once

	AVCodecContext* codecContext( _audioDesc.getCodecContext() );

	if( codecContext == NULL )
	{
		throw std::runtime_error( "could not allocate audio codec context" );
	}
	
	// try to open encoder with parameters.
	int ret = avcodec_open2( codecContext, _audioDesc.getCodec(), NULL );
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

	AVCodecContext* codecContext = _audioDesc.getCodecContext();

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
	AVCodecContext* codecContext = _audioDesc.getCodecContext();

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

void OutputAudio::setProfile( Profile::ProfileDesc& desc, const AudioFrameDesc& frameDesc  )
{
	if( ! desc.count( Profile::avProfileCodec ) || 		
		! desc.count( Profile::avProfileSampleFormat ) || 
		! desc.count( Profile::avProfileSampleRate ) || 
		! desc.count( Profile::avProfileChannel ) )
	{
		throw std::runtime_error( "The profile " + desc[ Profile::avProfileIdentificatorHuman ] + " is invalid." );
	}
	
	_audioDesc.setAudioCodec( desc[ Profile::avProfileCodec ] );
	
	size_t sample_rate = std::strtoul( desc[ Profile::avProfileSampleRate ].c_str(), NULL, 0 );
	size_t channels = std::strtoul( desc[ Profile::avProfileChannel ].c_str(), NULL, 0 );
	_audioDesc.setAudioParameters( sample_rate, channels, av_get_sample_fmt( desc[ Profile::avProfileSampleFormat ].c_str() ) );
	
	for( Profile::ProfileDesc::iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == Profile::avProfileIdentificator ||
			(*it).first == Profile::avProfileIdentificatorHuman ||
			(*it).first == Profile::avProfileType ||
			(*it).first == Profile::avProfileCodec ||
			(*it).first == Profile::avProfileSampleFormat ||
			(*it).first == Profile::avProfileSampleRate ||
			(*it).first == Profile::avProfileChannel )
			continue;

		try
		{
			_audioDesc.set( (*it).first, (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "warning: " << e.what() << std::endl;
		}
	}

	setup();

	for( Profile::ProfileDesc::iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == Profile::avProfileIdentificator ||
			(*it).first == Profile::avProfileIdentificatorHuman ||
			(*it).first == Profile::avProfileType ||
			(*it).first == Profile::avProfileCodec ||
			(*it).first == Profile::avProfileSampleFormat ||
			(*it).first == Profile::avProfileSampleRate ||
			(*it).first == Profile::avProfileChannel )
			continue;

		try
		{
			_audioDesc.set( (*it).first, (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "2.warning: " << e.what() << std::endl;
		}
	}
}

}

