#include "OutputFile.hpp"

#include <AvTranscoder/option/Context.hpp>

extern "C" {
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
	, _stream        ( NULL )
	, _filename      ( filename )
	, _packetCount   ( 0 )
	, _previousProcessedStreamDuration ( 0.0 )
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

IOutputStream& OutputFile::addVideoStream( const VideoCodec& videoDesc )
{
	assert( _formatContext != NULL );

	if( ( _stream = avformat_new_stream( _formatContext, videoDesc.getAVCodec() ) ) == NULL )
	{
		throw std::runtime_error( "unable to add new video stream" );
	}

	_stream->codec->width  = videoDesc.getAVCodecContext()->width;
	_stream->codec->height = videoDesc.getAVCodecContext()->height;
	_stream->codec->bit_rate = videoDesc.getAVCodecContext()->bit_rate;
	_stream->codec->pix_fmt = videoDesc.getAVCodecContext()->pix_fmt;
	_stream->codec->profile = videoDesc.getAVCodecContext()->profile;
	_stream->codec->level = videoDesc.getAVCodecContext()->level;

	// need to set the time_base on the AVCodecContext and the AVStream...
	av_reduce(
		&_stream->codec->time_base.num,
		&_stream->codec->time_base.den,
		videoDesc.getAVCodecContext()->time_base.num * videoDesc.getAVCodecContext()->ticks_per_frame,
		videoDesc.getAVCodecContext()->time_base.den,
		INT_MAX );

	_stream->time_base = _stream->codec->time_base;
	
	AvOutputStream* avOutputStream = new AvOutputStream( *this, _formatContext->nb_streams - 1 );

	_outputStreams.push_back( avOutputStream );

	return *_outputStreams.back();
}

IOutputStream& OutputFile::addAudioStream( const AudioCodec& audioDesc )
{
	assert( _formatContext != NULL );

	if( ( _stream = avformat_new_stream( _formatContext, audioDesc.getAVCodec() ) ) == NULL )
	{
		throw std::runtime_error( "unable to add new audio stream" );
	}

	_stream->codec->sample_rate = audioDesc.getAVCodecContext()->sample_rate;
	_stream->codec->channels = audioDesc.getAVCodecContext()->channels;
	_stream->codec->sample_fmt = audioDesc.getAVCodecContext()->sample_fmt;

	AvOutputStream* avOutputStream = new AvOutputStream( *this, _formatContext->nb_streams - 1 );
	_outputStreams.push_back( avOutputStream );

	return *_outputStreams.back();
}

IOutputStream& OutputFile::addDataStream( const DataCodec& dataDesc )
{
	assert( _formatContext != NULL );

	if( ( _stream = avformat_new_stream( _formatContext, dataDesc.getAVCodec() ) ) == NULL )
	{
		throw std::runtime_error( "unable to add new data stream" );
	}

	AvOutputStream* avOutputStream = new AvOutputStream( *this, _formatContext->nb_streams - 1 );
	_outputStreams.push_back( avOutputStream );

	return *_outputStreams.back();
}

IOutputStream& OutputFile::getStream( const size_t streamId )
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

IOutputStream::EWrappingStatus OutputFile::wrap( const CodedData& data, const size_t streamId )
{
	if( ! data.getSize() )
		return avtranscoder::IOutputStream::eWrappingError;
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
		return IOutputStream::eWrappingError;
	}

	av_free_packet( &packet );

	// get the current streams
	AVStream* currentStream = _formatContext->streams[ streamId ];
	// compute its duration
	double currentStreamDuration = (double)currentStream->cur_dts * currentStream->time_base.num / currentStream->time_base.den;
	
	if( currentStreamDuration < _previousProcessedStreamDuration )
	{
		// if the current stream is strictly shorter than the previous, wait for more data
		return IOutputStream::eWrappingWaitingForData;
	}

	_previousProcessedStreamDuration = currentStreamDuration;
	
	_packetCount++;
	_frameCount.at( streamId )++;
	return IOutputStream::eWrappingSuccess;
}

bool OutputFile::endWrap( )
{
	if( av_write_trailer( _formatContext ) != 0)
	{
		throw std::runtime_error( "could not write trailer" );
	}

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

void OutputFile::setProfile( const ProfileLoader::Profile& profile )
{
	if( ! profile.count( constants::avProfileFormat ) )
	{
		throw std::runtime_error( "The profile " + profile.find( constants::avProfileIdentificatorHuman )->second + " is invalid." );
	}
	
	if( ! matchFormat( profile.find( constants::avProfileFormat )->second, _filename ) )
	{
		throw std::runtime_error( "Invalid format according to the file extension." );
	}
	_outputFormat = av_guess_format( profile.find( constants::avProfileFormat )->second.c_str(), _filename.c_str(), NULL);
	
	Context formatContext( _formatContext );
	
	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType ||
			(*it).first == constants::avProfileFormat )
			continue;
		
		try
		{
			Option& formatOption = formatContext.getOption( (*it).first );
			formatOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			//std::cout << "[OutputFile] warning: " << e.what() << std::endl;
		}
	}
	
	setup();
	
	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType ||
			(*it).first == constants::avProfileFormat )
			continue;

		try
		{
			Option& formatOption = formatContext.getOption( (*it).first );
			formatOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[OutputFile] warning - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what() << std::endl;
		}
	}
}

}
