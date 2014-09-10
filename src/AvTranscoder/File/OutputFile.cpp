#include "OutputFile.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
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
	, _verbose       ( false )
{
	if( ( _formatContext = avformat_alloc_context() ) == NULL )
	{
		throw std::runtime_error( "unable to create format context" );
	}
}

bool OutputFile::setup()
{
	av_register_all();
	if( ! _outputFormat )
		_outputFormat = av_guess_format( NULL, _filename.c_str(), NULL);

	if( ! _outputFormat )
	{
		throw std::runtime_error( "unable to find format" );
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

OutputStream& OutputFile::addVideoStream( const VideoDesc& videoDesc )
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
	
	AvOutputStream* avOutputStream = new AvOutputStream( *this, _formatContext->nb_streams - 1 );

	_outputStreams.push_back( avOutputStream );

	return *_outputStreams.back();
}

OutputStream& OutputFile::addAudioStream( const AudioDesc& audioDesc )
{
	assert( _formatContext != NULL );

	if( ( _stream = avformat_new_stream( _formatContext, audioDesc.getCodec() ) ) == NULL )
	{
		throw std::runtime_error( "unable to add new audio stream" );
	}

	_stream->codec->sample_rate = audioDesc.getCodecContext()->sample_rate;
	_stream->codec->channels = audioDesc.getCodecContext()->channels;
	_stream->codec->sample_fmt = audioDesc.getCodecContext()->sample_fmt;

	AvOutputStream* avOutputStream = new AvOutputStream( *this, _formatContext->nb_streams - 1 );
	_outputStreams.push_back( avOutputStream );

	return *_outputStreams.back();
}

OutputStream& OutputFile::addDataStream( const DataDesc& dataDesc )
{
	assert( _formatContext != NULL );

	if( ( _stream = avformat_new_stream( _formatContext, dataDesc.getCodec() ) ) == NULL )
	{
		throw std::runtime_error( "unable to add new data stream" );
	}

	AvOutputStream* avOutputStream = new AvOutputStream( *this, _formatContext->nb_streams - 1 );
	_outputStreams.push_back( avOutputStream );

	return *_outputStreams.back();
}

OutputStream& OutputFile::getStream( const size_t streamId )
{
	if( streamId >= _outputStreams.size() )
		throw std::runtime_error( "unable to get output stream (out of range)" );
	return *_outputStreams.at( streamId );
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
	_frameCount.clear();
	_frameCount.resize( _outputStreams.size(), 0 );
	return true;
}

bool OutputFile::wrap( const DataStream& data, const size_t streamId )
{
	if( ! data.getSize() )
		return true;
	if( _verbose )
		std::cout << "wrap on stream " << streamId << " (" << data.getSize() << " bytes for frame " << _frameCount.at( streamId ) << ")" << std::endl;
	AVPacket packet;
	av_init_packet( &packet );

	//av_packet_from_data( &packet, (uint8_t*)data.getPtr(), data.getSize() );

	packet.stream_index = streamId;

	packet.data = (uint8_t*)data.getPtr();
	packet.size = data.getSize();
	// packet.dts = _frameCount.at( streamId );
	// packet.pts = ;

	// int ret = av_write_frame( _formatContext, &packet );
	int ret = av_interleaved_write_frame( _formatContext, &packet );

	if( ret != 0 )
	{
		char err[250];
		av_strerror( ret, err, 250);
		std::string msg = "error when writting packet in stream: ";
		msg += err;
		// throw std::runtime_error( msg );
		std::cout << msg << std::endl;
		return false;
	}

	av_free_packet( &packet );

	_packetCount++;
	_frameCount.at( streamId )++;
	return true;
}

bool OutputFile::endWrap( )
{
	if( av_write_trailer( _formatContext ) != 0)
	{
		throw std::runtime_error( "could not write trailer" );
	}
	avcodec_close( _stream->codec );
	if( !( _formatContext->oformat->flags & AVFMT_NOFILE ) )
	{
		avio_close( _formatContext->pb );
	}
	avformat_free_context( _formatContext );
	//freeFormat();
	
	return true;
}

void OutputFile::addMetadata( const MetadatasMap& dataMap )
{
	for( MetadatasMap::const_iterator it = dataMap.begin(); it != dataMap.end(); ++it )
	{
		addMetadata( it->first, it->second );
	}
}

void OutputFile::addMetadata( const std::string& key, const std::string& value )
{
	int ret = av_dict_set( &_formatContext->metadata, key.c_str(), value.c_str(), 0 );
	if( ret < 0 )
	{
		char err[250];
		av_strerror( ret, err, 250 );
		std::cout << err << std::endl;
	}
}

void OutputFile::setProfile( const Profile::ProfileDesc& desc )
{
	if( ! desc.count( Profile::avProfileFormat ) )
	{
		throw std::runtime_error( "The profile " + desc.find( Profile::avProfileIdentificatorHuman )->second + " is invalid." );
	}
	
	if( ! matchFormat( desc.find( Profile::avProfileFormat )->second, _filename ) )
	{
		throw std::runtime_error( "Invalid format according to the file extension." );
	}
	_outputFormat = av_guess_format( desc.find( Profile::avProfileFormat )->second.c_str(), _filename.c_str(), NULL);
	
	ParamSet paramSet( _formatContext );
	
	for( Profile::ProfileDesc::const_iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == Profile::avProfileIdentificator ||
			(*it).first == Profile::avProfileIdentificatorHuman ||
			(*it).first == Profile::avProfileType ||
			(*it).first == Profile::avProfileFormat )
			continue;
		
		try
		{
			paramSet.set( (*it).first, (*it).second );
		}
		catch( std::exception& e )
		{
			//std::cout << "[OutputFile] warning: " << e.what() << std::endl;
		}
	}
	
	setup();
	
	for( Profile::ProfileDesc::const_iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == Profile::avProfileIdentificator ||
			(*it).first == Profile::avProfileIdentificatorHuman ||
			(*it).first == Profile::avProfileType ||
			(*it).first == Profile::avProfileFormat )
			continue;

		try
		{
			paramSet.set( (*it).first, (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[OutputFile] warning: " << e.what() << std::endl;
		}
	}
}

}
