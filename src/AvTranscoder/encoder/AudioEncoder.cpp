#include "AudioEncoder.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <stdexcept>

namespace avtranscoder
{

AudioEncoder::AudioEncoder( const std::string& audioCodecName )
	: _codec( eCodecTypeEncoder, audioCodecName )
	, _frame( NULL )
{
#if LIBAVCODEC_VERSION_MAJOR > 54
	_frame = av_frame_alloc();
#else
	_frame = avcodec_alloc_frame();
#endif
}

AudioEncoder::~AudioEncoder()
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

void AudioEncoder::setup()
{
	_codec.open();
}

bool AudioEncoder::encodeFrame( const Frame& sourceFrame, Frame& codedFrame )
{
	AVCodecContext& avCodecContext = _codec.getAVCodecContext();

	// Set default frame parameters
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_unref( _frame );
#else
	avcodec_get_frame_defaults( _frame );
#endif

	const AudioFrame& sourceAudioFrame = static_cast<const AudioFrame&>( sourceFrame );
	
	_frame->nb_samples     = sourceAudioFrame.getNbSamples();
	_frame->format         = avCodecContext.sample_fmt;
	_frame->channel_layout = avCodecContext.channel_layout;
	
	// we calculate the size of the samples buffer in bytes
	int bufferSize = av_samples_get_buffer_size( NULL, avCodecContext.channels, _frame->nb_samples, avCodecContext.sample_fmt, 0 );
	if( bufferSize < 0 )
	{
		throw std::runtime_error( "Encode audio frame error: buffer size < 0 - " + getDescriptionFromErrorCode( bufferSize ) );
	}

	int retvalue = avcodec_fill_audio_frame( _frame, avCodecContext.channels, avCodecContext.sample_fmt, sourceAudioFrame.getData(), bufferSize, 0 );
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
	int ret = avcodec_encode_audio2( &avCodecContext, &packet, _frame, &gotPacket );
	if( ret != 0 && gotPacket == 0 )
	{
		throw std::runtime_error( "Encode audio frame error: avcodec encode audio frame - " + getDescriptionFromErrorCode( ret ) );
	}
#else
	int ret = avcodec_encode_audio( &avCodecContext, packet.data, packet.size, _frame );
	if( ret < 0 )
	{
		throw std::runtime_error( "Encode audio frame error: avcodec encode audio frame - " + getDescriptionFromErrorCode( ret ) );
	}
#endif
	
#if LIBAVCODEC_VERSION_MAJOR > 53
	return ret == 0 && gotPacket == 1;
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
	LOG_DEBUG( "Set profile of audio encoder with:\n" << profile )

	// set sampleRate, number of channels, sample format
	_codec.setAudioParameters( frameDesc );

	// set threads before any other options
	if( profile.count( constants::avProfileThreads ) )
		_codec.getOption( constants::avProfileThreads ).setString( profile.at( constants::avProfileThreads ) );
	else
		_codec.getOption( constants::avProfileThreads ).setInt( _codec.getAVCodecContext().thread_count );


	// set encoder options
	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType ||
			(*it).first == constants::avProfileCodec ||
			(*it).first == constants::avProfileSampleFormat ||
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

	setup();

	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType ||
			(*it).first == constants::avProfileCodec ||
			(*it).first == constants::avProfileSampleFormat ||
			(*it).first == constants::avProfileThreads )
			continue;

		try
		{
			Option& encodeOption = _codec.getOption( (*it).first );
			encodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			LOG_WARN( "AudioEncoder - can't set option " << (*it).first <<  " to " << (*it).second << ": " << e.what() )
		}
	}
}

}

