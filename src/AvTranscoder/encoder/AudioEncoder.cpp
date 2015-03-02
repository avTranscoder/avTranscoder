#include "AudioEncoder.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <iostream>
#include <stdexcept>

namespace avtranscoder
{

AudioEncoder::AudioEncoder( const std::string& audioCodecName )
	: _codec( eCodecTypeEncoder, audioCodecName )
{
}

void AudioEncoder::setup()
{
	_codec.open();
}

bool AudioEncoder::encodeFrame( const Frame& sourceFrame, Frame& codedFrame )
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

	const AudioFrame& sourceAudioFrame = static_cast<const AudioFrame&>( sourceFrame );
	
	frame->nb_samples     = sourceAudioFrame.getNbSamples();
	frame->format         = avCodecContext.sample_fmt;
	frame->channel_layout = avCodecContext.channel_layout;
	
	// we calculate the size of the samples buffer in bytes
	int bufferSize = av_samples_get_buffer_size( NULL, avCodecContext.channels, frame->nb_samples, avCodecContext.sample_fmt, 0 );
	if( bufferSize < 0 )
	{
		throw std::runtime_error( "Encode audio frame error: buffer size < 0 - " + getDescriptionFromErrorCode( bufferSize ) );
	}

	int retvalue = avcodec_fill_audio_frame( frame, avCodecContext.channels, avCodecContext.sample_fmt, sourceAudioFrame.getData(), bufferSize, 0 );
	if( retvalue < 0 )
	{
		throw std::runtime_error( "Encode audio frame error: avcodec fill audio frame - " + getDescriptionFromErrorCode( retvalue ) );
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
	int ret = avcodec_encode_audio2( &avCodecContext, &packet, frame, &gotPacket );
	if( ret != 0 && gotPacket == 0 )
	{
		throw std::runtime_error( "Encode audio frame error: avcodec encode audio frame - " + getDescriptionFromErrorCode( ret ) );
	}
#else
	int ret = avcodec_encode_audio( &avCodecContext, packet.data, packet.size, frame );
	if( ret < 0 )
	{
		throw std::runtime_error( "Encode audio frame error: avcodec encode audio frame - " + getDescriptionFromErrorCode( ret ) );
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

bool AudioEncoder::encodeFrame( Frame& codedFrame )
{
	AVCodecContext& avCodecContext = _codec.getAVCodecContext();

	AVPacket& packet = codedFrame.getAVPacket();
	packet.stream_index = 0;

#if LIBAVCODEC_VERSION_MAJOR > 53
	int gotPacket = 0;
	int ret = avcodec_encode_audio2( &avCodecContext, &packet, NULL, &gotPacket );
	if( ret != 0 && gotPacket == 0 )
	{
		throw std::runtime_error( "Encode audio frame error: avcodec encode last audio frame - " + getDescriptionFromErrorCode( ret ) );
	}
	return ret == 0 && gotPacket == 1;

#else
	int ret = avcodec_encode_audio( &avCodecContext, packet.data, packet.size, NULL );
	if( ret < 0 )
	{
		throw std::runtime_error( "Encode audio frame error: avcodec encode last audio frame - " + getDescriptionFromErrorCode( ret ) );
	}
	return ret == 0;

#endif
}

void AudioEncoder::setProfile( const ProfileLoader::Profile& profile, const AudioFrameDesc& frameDesc  )
{
	// set sampleRate, number of channels, sample format
	_codec.setAudioParameters( frameDesc );

	// set threads if not in profile
	if( ! profile.count( "threads" ) )
		_codec.getOption( "threads" ).setString( "auto" );

	// set encoder options
	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType ||
			(*it).first == constants::avProfileCodec ||
			(*it).first == constants::avProfileSampleFormat )
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
			(*it).first == constants::avProfileSampleFormat )
			continue;

		try
		{
			Option& encodeOption = _codec.getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[AudioEncoder] warning - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what() << std::endl;
		}
	}
}

}

