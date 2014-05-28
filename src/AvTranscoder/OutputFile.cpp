#include "OutputFile.hpp"

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

OutputFile::OutputFile( const std::string& file )
	: outputFormat  ( NULL )
	, formatContext ( NULL )
	, codec         ( NULL )
	, codecContext  ( NULL )
	, stream        ( NULL )
	, filename      ( file )
	, packetCount   ( 0 )
{
}

bool OutputFile::setup()
{
	av_register_all();
	outputFormat = av_guess_format( NULL, filename.c_str(), NULL);

	if( !outputFormat )
	{
		throw std::runtime_error( "unable to find format" );
	}

	if( ( formatContext = avformat_alloc_context() ) == NULL )
	{
		throw std::runtime_error( "unable to create format context" );
	}

	formatContext->oformat = outputFormat;

	if( !( outputFormat->flags & AVFMT_NOFILE ) )
	{
		if( avio_open2( &formatContext->pb, filename.c_str(), AVIO_FLAG_WRITE, NULL, NULL ) < 0 )
		{
			avformat_close_input( &formatContext );
			formatContext = NULL;
			throw std::runtime_error( "error when opening output format" );
		}
	}

	return formatContext != NULL;
}

void OutputFile::addVideoStream( const VideoDesc& videoDesc )
{
	assert( formatContext != NULL );

	if( ( stream = avformat_new_stream( formatContext, videoDesc.getCodec() ) ) == NULL )
	{
		throw std::runtime_error( "unable to add new video stream" );
	}

	// need to set the time_base on the AVCodecContext and the AVStream...
	stream->codec->width  = videoDesc.getCodecContext()->width;
	stream->codec->height = videoDesc.getCodecContext()->height;
	stream->codec->bit_rate = videoDesc.getCodecContext()->bit_rate;
	stream->codec->time_base = videoDesc.getCodecContext()->time_base;
	stream->codec->pix_fmt = videoDesc.getCodecContext()->pix_fmt;
	stream->codec->profile = videoDesc.getCodecContext()->profile;
	stream->codec->level = videoDesc.getCodecContext()->level;

	stream->time_base = stream->codec->time_base;
}

void OutputFile::addAudioStream( const AudioDesc& audioDesc )
{
	assert( formatContext != NULL );

	if( ( stream = avformat_new_stream( formatContext, audioDesc.getCodec() ) ) == NULL )
	{
		throw std::runtime_error( "unable to add new audio stream" );
	}

	stream->codec->sample_rate = audioDesc.getCodecContext()->sample_rate;
	stream->codec->channels = audioDesc.getCodecContext()->channels;
	stream->codec->sample_fmt = audioDesc.getCodecContext()->sample_fmt;
}

bool OutputFile::beginWrap( )
{
	if( avformat_write_header( formatContext, NULL ) != 0 )
	{
		throw std::runtime_error( "could not write header" );
	}
	return true;
}

bool OutputFile::wrap( const DataStream& data, const size_t streamId )
{
	AVPacket packet;
	av_init_packet( &packet );

	//av_packet_from_data( &packet, (uint8_t*)data.getPtr(), data.getSize() );

	packet.stream_index = streamId;

	packet.data = (uint8_t*)data.getPtr();
	packet.size = data.getSize();
	packet.dts = 0;
	packet.pts = packetCount;

	if( av_interleaved_write_frame( formatContext, &packet ) != 0 )
	{
		std::cout << "error when writting packet in stream" << std::endl;
		return false;
	}

	av_free_packet( &packet );

	packetCount++;
	return true;
}

bool OutputFile::endWrap( )
{
	return true;
}

}
