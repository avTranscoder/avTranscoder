#include "InputStreamVideo.hpp"

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

InputStreamVideo::InputStreamVideo( const std::string& filename, const size_t streamIndex )
	: formatContext( NULL )
	, codec        ( NULL )
	, codecContext ( NULL )
	, selectedStream( -1 )
	, width  ( 0 )
	, height ( 0 )
	, components ( 0 )
	, bitDepth ( 0 )
{}

bool InputStreamVideo::setup( const std::string& filename, const size_t streamIndex )
{
	av_register_all();
	//av_log_set_level( AV_LOG_FATAL );
	
	if( avformat_open_input( &formatContext, filename.c_str(), NULL, NULL ) < 0 )
	{
		return false;
	}

	// update format context informations from streams
	if( avformat_find_stream_info( formatContext, NULL ) < 0 )
	{
		return false;
	}

	size_t videoStreamCount = 0;
	for( size_t streamId = 0; streamId < formatContext->nb_streams; streamId++ )
	{
		if( formatContext->streams[streamId]->codec->codec_type == AVMEDIA_TYPE_VIDEO )
		{
			if( videoStreamCount == streamIndex )
			{
				selectedStream = streamId;
			}
			videoStreamCount++;
		}
	}

	if( selectedStream == -1 )
		return false;

	codec = avcodec_find_decoder( formatContext->streams[selectedStream]->codec->codec_id );
	if( codec == NULL )
	{
		return false;
	}

	codecContext = avcodec_alloc_context3( codec );

	if( codecContext == NULL )
	{
		return false;
	}

	if( codec->capabilities & CODEC_CAP_TRUNCATED )
		codecContext->flags|= CODEC_FLAG_TRUNCATED;

	avcodec_open2( codecContext, codec, NULL );

	if( codecContext == NULL || codec == NULL )
	{
		return false;
	}

	const AVPixFmtDescriptor* pixFmt = av_pix_fmt_desc_get( codecContext->pix_fmt );
	if( pixFmt )
	{
		components = pixFmt->nb_components;
		bitDepth = pixFmt->comp[0].depth_minus1 + 1;
	}
	width  = formatContext->streams[selectedStream]->codec->width;
	height = formatContext->streams[selectedStream]->codec->height;

	return true;
}

std::vector<unsigned char>& InputStreamVideo::readNextFrame( std::vector<unsigned char>& frameBuffer )
{
	AVPacket pkt;
	AVFrame* frame = avcodec_alloc_frame();

	int got_frame = 0;

	while( ! got_frame )
	{
		av_init_packet( &pkt );
		av_read_frame( formatContext, &pkt );
		// We only read one stream and skip others
		if( pkt.stream_index == selectedStream )
		{
			avcodec_decode_video2( codecContext, frame, &got_frame, &pkt );
		}
		av_free_packet( &pkt );
	}

	//std::cout << "read frame " << frame->width << "x"<< frame->height << " disp time " << frame->display_picture_number << "/" << frame->coded_picture_number << std::endl;

	frameBuffer.resize( avpicture_get_size( (AVPixelFormat)frame->format, frame->width, frame->height ) );
	//memcpy( &frameBuffer[0], frame->data, frameBuffer.size() );

	avpicture_layout( (AVPicture*)frame, (AVPixelFormat)frame->format, frame->width, frame->height, &frameBuffer[0], frameBuffer.size() );

	//av_frame_free( &frame );

	return frameBuffer;
}

}