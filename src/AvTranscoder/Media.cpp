#include "Media.hpp"

#include "mediaStreams/VideoStreamProperties.hpp"
#include "mediaStreams/AudioStreamProperties.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <iostream>

namespace avtranscoder
{

Media::Media( const std::string& inputFile )
	: filename( inputFile )
	, formatContext( NULL )
{}

bool Media::analyse()
{
	av_register_all();
	//av_log_set_level( AV_LOG_DEBUG );
	
	if( avformat_open_input( &formatContext, filename.c_str(), NULL, NULL ) < 0 )
	{
		std::cerr << "Could not open file" << std::endl;
		return false;
	}
	
	if( avformat_find_stream_info( formatContext, NULL ) < 0 )
	{
		std::cerr << "Could not find file info" << std::endl;
		return false;
	}

	properties.filename = formatContext->filename;
	properties.formatName = formatContext->iformat->name;
	properties.formatLongName = formatContext->iformat->long_name;
	properties.streamsCount = formatContext->nb_streams;
	properties.programsCount = formatContext->nb_programs;
	properties.startTime = 1.0 * (uint)formatContext->start_time / AV_TIME_BASE;
	properties.duration = 1.0 * formatContext->duration / AV_TIME_BASE;
	properties.bitRate = formatContext->bit_rate;
	properties.packetSize = formatContext->packet_size;

	AVDictionaryEntry *tag = NULL;
	while( ( tag = av_dict_get( formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX ) ) )
	{
		properties.metadatas.push_back( std::pair<std::string, std::string>( tag->key, tag->value ) );
	}

	for( size_t streamId = 0; streamId < formatContext->nb_streams; streamId++ )
	{
		switch( formatContext->streams[streamId]->codec->codec_type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				properties.videoStreams.push_back( videoStreamInfo( formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				properties.audioStreams.push_back( audioStreamInfo( formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_UNKNOWN:
			{
				std::cout << "unknown stream" << std::endl;
				break;
			}
			case AVMEDIA_TYPE_DATA:
			{
				std::cout << "data stream" << std::endl;
				break;
			}
			case AVMEDIA_TYPE_SUBTITLE:
			{
				std::cout << "subtitle stream" << std::endl;
				break;
			}
			case AVMEDIA_TYPE_ATTACHMENT:
			{
				std::cout << "attachement" << std::endl;
				break;
			}
			case AVMEDIA_TYPE_NB:
			{
				std::cout << "NB" << std::endl;
				break;
			}
		}
	}

	return true;
}

}
