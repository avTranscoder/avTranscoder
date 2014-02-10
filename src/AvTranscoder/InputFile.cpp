#include "InputFile.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/avstring.h>
}

#include <iostream>
#include <stdexcept>

namespace avtranscoder
{

InputFile::InputFile()
	: m_formatContext ( NULL )
	, m_codec         ( NULL )
	, m_codecContext  ( NULL )
	, m_stream        ( NULL )
	, m_filename      ( "" )
	, m_packetCount   ( 0 )
{
	av_register_all();  // Warning: should be called only once
}

InputFile::~InputFile()
{
	if( m_formatContext )
	{
		avformat_close_input( &m_formatContext );
		m_formatContext = NULL;
	}
	if( m_codecContext )
	{
		avcodec_close( m_codecContext );
		m_codecContext = NULL;
	}
}

InputFile& InputFile::setup( const std::string& filename )
{
	m_filename = filename;
	if( avformat_open_input( &m_formatContext, m_filename.c_str(), NULL, NULL ) < 0 )
	{
		throw std::runtime_error( "unable to open file" );
	}

	// update format context informations from streams
	if( avformat_find_stream_info( m_formatContext, NULL ) < 0 )
	{
		avformat_close_input( &m_formatContext );
		m_formatContext = NULL;
		throw std::runtime_error( "unable to find stream informations" );
	}

	return *this;
}

VideoDesc InputFile::getVideoDesc( size_t videoStreamId )
{
	int selectedStream = -1;
	size_t videoStreamCount = 0;

	for( size_t streamId = 0; streamId < m_formatContext->nb_streams; streamId++ )
	{
		if( m_formatContext->streams[streamId]->codec->codec_type == AVMEDIA_TYPE_VIDEO )
		{
			if( videoStreamCount == videoStreamId )
			{
				selectedStream = streamId;
			}
			videoStreamCount++;
		}
	}

	if( selectedStream == -1 )
		throw std::runtime_error( "unable to find video stream" );

	AVCodecContext* codecContext = m_formatContext->streams[selectedStream]->codec;

	VideoDesc desc( codecContext->codec_id );

	desc.setImageParameters( codecContext->width, codecContext->height, codecContext->pix_fmt );
	desc.setTimeBase( codecContext->time_base.num, codecContext->time_base.den );

	return desc;
}

bool InputFile::unwrap( DataStream& data, const size_t streamIndex )
{
	AVPacket packet;
	av_init_packet( &packet );

	readNextPacket( packet, streamIndex );

	data.getBuffer().resize( packet.size );
	memcpy( data.getPtr(), packet.data, packet.size );

	av_free_packet( &packet );

	return true;
}

bool InputFile::readNextPacket( AVPacket& packet, const size_t streamIndex )
{
	while( 1 )
	{
		int ret = av_read_frame( m_formatContext, &packet );
		if( ret < 0 ) // error or end of file
		{
			return false;
		}

		// We only read one stream and skip others
		if( packet.stream_index == (int)streamIndex )
		{
			return true;
		}
	}
}

}
