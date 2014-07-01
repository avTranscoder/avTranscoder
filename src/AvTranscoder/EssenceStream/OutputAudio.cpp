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
#include <sstream>

namespace avtranscoder
{

OutputAudio::OutputAudio()
	: OutputEssence()
	, _audioDesc( "pcm_s16le" )
{
}

bool OutputAudio::setup()
{
	av_register_all();  // Warning: should be called only once

	AVCodecContext* codecContext( _audioDesc.getCodecContext() );

	if( codecContext == NULL )
		return false;
	
	// try to open encoder with parameters.
	if( avcodec_open2( codecContext, _audioDesc.getCodec(), NULL ) < 0 )
		return false;

	return true;
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

void OutputAudio::setProfile( Profile::ProfileDesc& desc )
{
	checkProfileKey( desc, "codec" );
	checkProfileKey( desc, "sample_fmt" );
	checkProfileKey( desc, "sample_rate" );
	checkProfileKey( desc, "channels" );

	size_t sample_rate;
	size_t channels;
	std::istringstream( desc["sample_rate"] ) >> sample_rate;
	std::istringstream( desc["channels"] ) >> channels;

	_audioDesc.setAudioCodec( desc["codec"] );
	_audioDesc.setAudioParameters( sample_rate, channels, av_get_sample_fmt( desc["sample_fmt"].c_str() ) );

	setup();
}

bool OutputAudio::checkProfileKey( Profile::ProfileDesc& profDesc, const std::string& key )
{
	if( profDesc[ key ].empty() )
	{
		throw std::runtime_error( key + " not specified into profile: " + profDesc[ Profile::avProfileIdentificator ] );
		return false;
	}
	return true;
}

}

