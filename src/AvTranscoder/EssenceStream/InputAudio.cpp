#include "InputAudio.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
}

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

#include <iostream>
#include <stdexcept>

namespace avtranscoder
{

InputAudio::InputAudio( AvInputStream& inputStream ) 
	: InputEssence   ( inputStream )
	, _inputStream   ( &inputStream )
	, _codec         ( NULL )
	, _codecContext  ( NULL )
	, _frame         ( NULL )
	, _selectedStream( -1 )
{
}

InputAudio::~InputAudio()
{
	if( _codecContext != NULL )
	{
		avcodec_close( _codecContext );
		av_free( _codecContext );
		_codecContext = NULL;
	}
	if( _frame != NULL )
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
		_frame = NULL;
	}
}


void InputAudio::setup()
{
	avcodec_register_all();

	_codec = avcodec_find_decoder( _inputStream->getAudioDesc().getAudioCodecId() );
	if( _codec == NULL )
	{
		throw std::runtime_error( "codec not supported" );
	}
	
	_codecContext = avcodec_alloc_context3( _codec );
	if( _codecContext == NULL )
	{
		throw std::runtime_error( "unable to find context for codec" );
	}
	
	_codecContext->channels = _inputStream->getAudioDesc().getChannels();
	
	// std::cout << "Audio codec Id : " << _codecContext->codec_id << std::endl;
	// std::cout << "Audio codec Id : " << _codec->name << std::endl;
	// std::cout << "Audio codec Id : " << _codec->long_name << std::endl;

	_codecContext->channels = _inputStream->getAudioDesc().getCodecContext()->channels;
	
	int ret = avcodec_open2( _codecContext, _codec, NULL );

	if( ret < 0 || _codecContext == NULL || _codec == NULL )
	{
		std::string msg = "unable open audio codec: ";
		msg +=  _codec->long_name;
		msg += " (";
		msg += _codec->name;
		msg += ")";
		avcodec_close( _codecContext );

		char err[250];

		av_strerror( ret, err, 250 );
		std::cout << err << std::endl;
		throw std::runtime_error( msg );
	}

#if LIBAVCODEC_VERSION_MAJOR > 54
	_frame = av_frame_alloc();
#else
	_frame = avcodec_alloc_frame();
#endif
	if( _frame == NULL )
	{
		throw std::runtime_error( "unable to setup frame buffer" );
	}
}

bool InputAudio::readNextFrame( Frame& frameBuffer )
{
	if( ! getNextFrame() )
		return false;
	
	size_t decodedSize = av_samples_get_buffer_size(NULL, _codecContext->channels,
													_frame->nb_samples,
													_codecContext->sample_fmt, 1);
	
	AudioFrame& audioBuffer = static_cast<AudioFrame&>( frameBuffer );

	audioBuffer.setNbSamples( _frame->nb_samples );
	
	if( decodedSize )
	{
		if( audioBuffer.getSize() != decodedSize )
			audioBuffer.getBuffer().resize( decodedSize, 0 );
		
		unsigned char* dst = audioBuffer.getPtr();
		av_samples_copy(&dst, (uint8_t* const* )_frame->data, 0,
						0, _frame->nb_samples, _codecContext->channels,
						_codecContext->sample_fmt);
	}

	return true;
}

bool InputAudio::readNextFrame( std::vector<Frame>& frameBuffer )
{
	if( ! getNextFrame() )
		return false;

	size_t nbChannels = av_get_channel_layout_nb_channels( _frame->channel_layout );
	size_t bytePerSample = av_get_bytes_per_sample( (AVSampleFormat)_frame->format );

	frameBuffer.resize( nbChannels );

	for( size_t channel = 0; channel < nbChannels; ++ channel )
	{
		AudioFrame& audioBuffer = static_cast<AudioFrame&>( frameBuffer.at( channel ) );
		audioBuffer.setNbSamples( _frame->nb_samples );

		unsigned char* src = *_frame->data;
		unsigned char* dst = audioBuffer.getPtr();

		for( int sample = 0; sample < _frame->nb_samples; ++sample )
		{
			memcpy( dst, src, bytePerSample );
			dst += bytePerSample;
			src += bytePerSample * nbChannels;
		}
	}
	return true;
}

bool InputAudio::getNextFrame()
{
	int got_frame = 0;
	while( ! got_frame )
	{
		DataStream data;
		if( ! _inputStream->readNextPacket( data ) ) // error or end of file
			return false;

		AVPacket packet;
		av_init_packet( &packet );
		
		packet.stream_index = _selectedStream;
		packet.data         = data.getPtr();
		packet.size         = data.getSize();
		
		int ret = avcodec_decode_audio4( _codecContext, _frame, &got_frame, &packet );

		if( ret < 0 )
		{
			char err[250];
			av_strerror( ret, err, 250);
			
			throw std::runtime_error( "an error occured during audio decoding" + std::string( err ) );
		}

		av_free_packet( &packet );
	}
	return true;
}

}
