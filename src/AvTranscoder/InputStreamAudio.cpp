#include "InputStreamAudio.hpp"
#include "common.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include "AvInputStream.hpp"

#include <iostream>
#include <stdexcept>

namespace avtranscoder
{

InputStreamAudio::InputStreamAudio( AvInputStream& inputStream )
	: m_inputStream   ( &inputStream )
	, m_codec         ( NULL )
	, m_codecContext  ( NULL )
	, m_frame         ( NULL )
	, m_selectedStream( -1 )
{
	avcodec_register_all();

	m_codec = avcodec_find_decoder( m_inputStream->getAudioDesc().getAudioCodecId() );
	if( m_codec == NULL )
	{
		throw std::runtime_error( "codec not supported" );
	}
	
	m_codecContext = avcodec_alloc_context3( m_codec );
	if( m_codecContext == NULL )
	{
		throw std::runtime_error( "unable to find context for codec" );
	}
	
	m_codecContext->channels = m_inputStream->getAudioDesc().getChannels();

	std::cout << "Audio codec Id : " << m_codecContext->codec_id << std::endl;
	std::cout << "Audio codec Id : " << m_codec->name << std::endl;
	std::cout << "Audio codec Id : " << m_codec->long_name << std::endl;

	m_codecContext->channels = m_inputStream->getAudioDesc().getCodecContext()->channels;
	
	int ret = avcodec_open2( m_codecContext, m_codec, NULL );

	std::cout << "ret value : " << ret << std::endl;

	if( ret < 0 || m_codecContext == NULL || m_codec == NULL )
	{
		std::string msg = "unable open audio codec: ";
		msg +=  m_codec->long_name;
		msg += " (";
		msg += m_codec->name;
		msg += ")";
		avcodec_close( m_codecContext );

		char err[250];

		av_strerror( ret, err, 250 );
		std::cout << err << std::endl;
		throw std::runtime_error( msg );
	}

#if LIBAVCODEC_VERSION_MAJOR > 54
	m_frame = av_frame_alloc();
#else
	m_frame = avcodec_alloc_frame();
#endif
	if( m_frame == NULL )
	{
		throw std::runtime_error( "unable to setup frame buffer" );
	}
}

InputStreamAudio::~InputStreamAudio()
{
	if( m_codecContext != NULL )
	{
		avcodec_close( m_codecContext );
		av_free( m_codecContext );
		m_codecContext = NULL;
	}
	if( m_frame != NULL )
	{
#if LIBAVCODEC_VERSION_MAJOR > 54
		av_frame_free( &m_frame );
#else
 #if LIBAVCODEC_VERSION_MAJOR > 53
		avcodec_free_frame( &m_frame );
 #else
		av_free( m_frame );
 #endif
#endif
		m_frame = NULL;
	}
}

bool InputStreamAudio::readNextFrame( AudioFrame& audioFrameBuffer )
{	
	int got_frame = 0;
	while( ! got_frame )
	{
		DataStream data;
		if( ! m_inputStream->readNextPacket( data ) ) // error or end of file
			return false;

		AVPacket packet;
		av_init_packet( &packet );
		
		packet.stream_index = m_selectedStream;
		packet.data         = data.getPtr();
		packet.size         = data.getSize();
		
		int ret = avcodec_decode_audio4( m_codecContext, m_frame, &got_frame, &packet );

		if( ret < 0 )
		{
			throw std::runtime_error( "an error occured during audio decoding" );
		}

		av_free_packet( &packet );
	}
	
	size_t decodedSize = av_samples_get_buffer_size(NULL, m_codecContext->channels,
													m_frame->nb_samples,
													m_codecContext->sample_fmt, 1);
	
	if( decodedSize )
	{
		if( audioFrameBuffer.getSize() != decodedSize )
			audioFrameBuffer.getBuffer().resize( decodedSize );

		int nb_channels = av_get_channel_layout_nb_channels( *m_codecContext->codec->channel_layouts );
		
		unsigned char* dest = audioFrameBuffer.getPtr();
		av_samples_fill_arrays(&dest, m_frame->linesize,
							   m_frame->data[0],
							   nb_channels, m_frame->nb_samples,
							   m_codecContext->sample_fmt, 1);
	}

	return true;
}

}
