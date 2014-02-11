#include "InputFile.hpp"
#include "Metadatas/VideoStreamProperties.hpp"
#include "Metadatas/AudioStreamProperties.hpp"
#include "Metadatas/DataStreamProperties.hpp"
#include "Metadatas/SubtitleStreamProperties.hpp"
#include "Metadatas/AttachementStreamProperties.hpp"
#include "Metadatas/UnknownStreamProperties.hpp"


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
	, m_stream        ( NULL )
	, m_filename      ( "" )
	, m_packetCount   ( 0 )
{
	av_register_all();  // Warning: should be called only once
}

InputFile::~InputFile()
{
	if( m_formatContext != NULL )
	{
		avformat_close_input( &m_formatContext );
		m_formatContext = NULL;
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

InputFile& InputFile::analyse()
{
	properties.filename = m_formatContext->filename;
	properties.formatName = m_formatContext->iformat->name;
	properties.formatLongName = m_formatContext->iformat->long_name;
	properties.streamsCount = m_formatContext->nb_streams;
	properties.programsCount = m_formatContext->nb_programs;
	properties.startTime = 1.0 * (uint)m_formatContext->start_time / AV_TIME_BASE;
	properties.duration = 1.0 * m_formatContext->duration / AV_TIME_BASE;
	properties.bitRate = m_formatContext->bit_rate;
	properties.packetSize = m_formatContext->packet_size;

	AVDictionaryEntry *tag = NULL;
	while( ( tag = av_dict_get( m_formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX ) ) )
	{
		properties.metadatas.push_back( std::pair<std::string, std::string>( tag->key, tag->value ) );
	}

	for( size_t streamId = 0; streamId < m_formatContext->nb_streams; streamId++ )
	{
		switch( m_formatContext->streams[streamId]->codec->codec_type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				properties.videoStreams.push_back( videoStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				properties.audioStreams.push_back( audioStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_DATA:
			{
				properties.dataStreams.push_back( dataStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_SUBTITLE:
			{
				properties.subtitleStreams.push_back( subtitleStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_ATTACHMENT:
			{
				properties.attachementStreams.push_back( attachementStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_UNKNOWN:
			{
				properties.unknownStreams.push_back( unknownStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_NB:
			{
				// std::cout << "NB" << std::endl;
				break;
			}
		}
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
