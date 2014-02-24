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
#include <cassert>

namespace avtranscoder
{

InputFile::InputFile( const std::string& filename )
	: m_formatContext ( NULL )
	, m_filename      ( filename )
{
	av_register_all();  // Warning: should be called only once
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

	for( size_t streamIndex = 0; streamIndex < m_formatContext->nb_streams; ++streamIndex )
	{
		m_inputStreams.push_back( InputStream( m_filename, streamIndex ) );
	}
}

InputFile::~InputFile()
{
	if( m_formatContext != NULL )
	{
		avformat_close_input( &m_formatContext );
		m_formatContext = NULL;
	}
}

InputFile& InputFile::analyse()
{
	assert( m_formatContext != NULL );

	m_properties.filename = m_formatContext->filename;
	m_properties.formatName = m_formatContext->iformat->name;
	m_properties.formatLongName = m_formatContext->iformat->long_name;
	m_properties.streamsCount = m_formatContext->nb_streams;
	m_properties.programsCount = m_formatContext->nb_programs;
	m_properties.startTime = 1.0 * (unsigned int)m_formatContext->start_time / AV_TIME_BASE;
	m_properties.duration = 1.0 * m_formatContext->duration / AV_TIME_BASE;
	m_properties.bitRate = m_formatContext->bit_rate;
	m_properties.packetSize = m_formatContext->packet_size;

	AVDictionaryEntry *tag = NULL;
	while( ( tag = av_dict_get( m_formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX ) ) )
	{
		m_properties.metadatas.push_back( std::pair<std::string, std::string>( tag->key, tag->value ) );
	}

	for( size_t streamId = 0; streamId < m_formatContext->nb_streams; streamId++ )
	{
		switch( m_formatContext->streams[streamId]->codec->codec_type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				m_properties.videoStreams.push_back( videoStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				m_properties.audioStreams.push_back( audioStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_DATA:
			{
				m_properties.dataStreams.push_back( dataStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_SUBTITLE:
			{
				m_properties.subtitleStreams.push_back( subtitleStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_ATTACHMENT:
			{
				m_properties.attachementStreams.push_back( attachementStreamInfo( m_formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_UNKNOWN:
			{
				m_properties.unknownStreams.push_back( unknownStreamInfo( m_formatContext, streamId ) );
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

InputStream& InputFile::getStream( size_t index )
{
	return m_inputStreams.at( index );
}

}
