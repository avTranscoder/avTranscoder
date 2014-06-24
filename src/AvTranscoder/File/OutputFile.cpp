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

OutputFile::OutputFile( const std::string& filename )
	: _outputFormat  ( NULL )
	, _formatContext ( NULL )
	, _codec         ( NULL )
	, _codecContext  ( NULL )
	, _stream        ( NULL )
	, _filename      ( filename )
	, _packetCount   ( 0 )
{
}

bool OutputFile::setup()
{
	av_register_all();
	_outputFormat = av_guess_format( NULL, _filename.c_str(), NULL);

	if( ! _outputFormat )
	{
		throw std::runtime_error( "unable to find format" );
	}

	if( ( _formatContext = avformat_alloc_context() ) == NULL )
	{
		throw std::runtime_error( "unable to create format context" );
	}

	_formatContext->oformat = _outputFormat;

	if( !( _outputFormat->flags & AVFMT_NOFILE ) )
	{
		if( avio_open2( &_formatContext->pb, _filename.c_str(), AVIO_FLAG_WRITE, NULL, NULL ) < 0 )
		{
			avformat_close_input( &_formatContext );
			_formatContext = NULL;
			throw std::runtime_error( "error when opening output format" );
		}
	}

	return _formatContext != NULL;
}

AvOutputStream& OutputFile::addVideoStream( const VideoDesc& videoDesc )
{
	assert( _formatContext != NULL );

	if( ( _stream = avformat_new_stream( _formatContext, videoDesc.getCodec() ) ) == NULL )
	{
		throw std::runtime_error( "unable to add new video stream" );
	}

	_stream->codec->width  = videoDesc.getCodecContext()->width;
	_stream->codec->height = videoDesc.getCodecContext()->height;
	_stream->codec->bit_rate = videoDesc.getCodecContext()->bit_rate;
	_stream->codec->ticks_per_frame = videoDesc.getCodecContext()->ticks_per_frame;
	_stream->codec->pix_fmt = videoDesc.getCodecContext()->pix_fmt;
	_stream->codec->profile = videoDesc.getCodecContext()->profile;
	_stream->codec->level = videoDesc.getCodecContext()->level;

	// need to set the time_base on the AVCodecContext and the AVStream...
	av_reduce(
		&_stream->codec->time_base.num,
		&_stream->codec->time_base.den,
		videoDesc.getCodecContext()->time_base.num * videoDesc.getCodecContext()->ticks_per_frame,
		videoDesc.getCodecContext()->time_base.den,
		INT_MAX );

	_stream->time_base = _stream->codec->time_base;
	
	_outputStreams.push_back( AvOutputStream( *this, _formatContext->nb_streams ) );

	return _outputStreams.back();
}

AvOutputStream& OutputFile::addAudioStream( const AudioDesc& audioDesc )
{
	assert( _formatContext != NULL );

	if( ( _stream = avformat_new_stream( _formatContext, audioDesc.getCodec() ) ) == NULL )
	{
		throw std::runtime_error( "unable to add new audio stream" );
	}

	_stream->codec->sample_rate = audioDesc.getCodecContext()->sample_rate;
	_stream->codec->channels = audioDesc.getCodecContext()->channels;
	_stream->codec->sample_fmt = audioDesc.getCodecContext()->sample_fmt;

	_outputStreams.push_back( AvOutputStream( *this, _formatContext->nb_streams ) );

	return _outputStreams.back();
}

AvOutputStream& OutputFile::getStream( const size_t streamId )
{
	return _outputStreams.at( streamId );
}

bool OutputFile::beginWrap( )
{
	int ret = avformat_write_header( _formatContext, NULL );
	if( ret != 0 )
	{
		char err[250];
		av_strerror( ret, err, 250);
		std::string msg = "could not write header: ";
		msg += err;
		throw std::runtime_error( msg );
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
	packet.pts = _packetCount;

	if( av_interleaved_write_frame( _formatContext, &packet ) != 0 )
	{
		std::cout << "error when writting packet in stream" << std::endl;
		return false;
	}

	av_free_packet( &packet );

	_packetCount++;
	return true;
}

bool OutputFile::endWrap( )
{
	return true;
}

}
