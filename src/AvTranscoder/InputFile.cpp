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

InputFile::InputFile( const std::string& file )
	: m_inputFormat   ( NULL )
	, m_formatContext ( NULL )
	, m_codec         ( NULL )
	, m_codecContext  ( NULL )
	, m_stream        ( NULL )
	, m_filename      ( file )
	, m_packetCount   ( 0 )
{
	av_register_all();  // Warning: should be called only once
}

bool InputFile::setup()
{
	av_register_all();

	if( avformat_open_input( &m_formatContext, m_filename.c_str(), NULL, NULL ) < 0 )
	{
		return false;
	}

	// update format context informations from streams
	if( avformat_find_stream_info( m_formatContext, NULL ) < 0 )
	{
		return false;
	}

	return m_formatContext != NULL;
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

bool unwrap( const Image& data, const size_t streamId )
{
	
}

}
