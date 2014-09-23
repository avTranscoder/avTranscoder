#include "InputFile.hpp"

#include <AvTranscoder/Metadatas/VideoStreamProperties.hpp>
#include <AvTranscoder/Metadatas/AudioStreamProperties.hpp>
#include <AvTranscoder/Metadatas/DataStreamProperties.hpp>
#include <AvTranscoder/Metadatas/SubtitleStreamProperties.hpp>
#include <AvTranscoder/Metadatas/AttachementStreamProperties.hpp>
#include <AvTranscoder/Metadatas/UnknownStreamProperties.hpp>


extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>
#include <cassert>

namespace avtranscoder
{

InputFile::InputFile( const std::string& filename )
	: _formatContext ( NULL )
	, _filename      ( filename )
{
	av_register_all();  // Warning: should be called only once
	if( avformat_open_input( &_formatContext, _filename.c_str(), NULL, NULL ) < 0 )
	{
		std::string msg = "unable to open file: ";
		msg += _filename;
		throw std::runtime_error( msg );
	}

	// update format context informations from streams
	if( avformat_find_stream_info( _formatContext, NULL ) < 0 )
	{
		avformat_close_input( &_formatContext );
		_formatContext = NULL;
		throw std::runtime_error( "unable to find stream informations" );
	}

	for( size_t streamIndex = 0; streamIndex < _formatContext->nb_streams; ++streamIndex )
	{
		_inputStreams.push_back( new AvInputStream( *this, streamIndex ) );
	}
}

InputFile::~InputFile()
{
	for( std::vector< AvInputStream* >::iterator it = _inputStreams.begin(); it != _inputStreams.end(); ++it )
	{
		delete (*it);
	}

	if( _formatContext != NULL )
	{
		avformat_close_input( &_formatContext );
		_formatContext = NULL;
	}
}

InputFile& InputFile::analyse( IProgress& progress, const EAnalyseLevel level )
{
	assert( _formatContext != NULL );

	seekAtFrame( 0 );

	_properties.filename = _formatContext->filename;
	_properties.formatName = _formatContext->iformat->name;
	_properties.formatLongName = _formatContext->iformat->long_name;
	_properties.streamsCount = _formatContext->nb_streams;
	_properties.programsCount = _formatContext->nb_programs;
	_properties.startTime = 1.0 * (unsigned int)_formatContext->start_time / AV_TIME_BASE;
	_properties.duration = 1.0 * _formatContext->duration / AV_TIME_BASE;
	_properties.bitRate = _formatContext->bit_rate;
	_properties.packetSize = _formatContext->packet_size;

	detail::fillMetadataDictionnary( _formatContext->metadata, _properties.metadatas );

	for( size_t streamId = 0; streamId < _formatContext->nb_streams; streamId++ )
	{
		switch( _formatContext->streams[streamId]->codec->codec_type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				_properties.videoStreams.push_back( videoStreamInfo( _formatContext, streamId, progress, level ) );
				detail::fillMetadataDictionnary( _formatContext->streams[streamId]->metadata, _properties.videoStreams.back().metadatas );
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				_properties.audioStreams.push_back( audioStreamInfo( _formatContext, streamId ) );
				detail::fillMetadataDictionnary( _formatContext->streams[streamId]->metadata, _properties.audioStreams.back().metadatas );
				break;
			}
			case AVMEDIA_TYPE_DATA:
			{
				_properties.dataStreams.push_back( dataStreamInfo( _formatContext, streamId ) );
				detail::fillMetadataDictionnary( _formatContext->streams[streamId]->metadata, _properties.dataStreams.back().metadatas );
				break;
			}
			case AVMEDIA_TYPE_SUBTITLE:
			{
				_properties.subtitleStreams.push_back( subtitleStreamInfo( _formatContext, streamId ) );
				detail::fillMetadataDictionnary( _formatContext->streams[streamId]->metadata, _properties.subtitleStreams.back().metadatas );
				break;
			}
			case AVMEDIA_TYPE_ATTACHMENT:
			{
				_properties.attachementStreams.push_back( attachementStreamInfo( _formatContext, streamId ) );
				detail::fillMetadataDictionnary( _formatContext->streams[streamId]->metadata, _properties.attachementStreams.back().metadatas );
				break;
			}
			case AVMEDIA_TYPE_UNKNOWN:
			{
				_properties.unknownStreams.push_back( unknownStreamInfo( _formatContext, streamId ) );
				detail::fillMetadataDictionnary( _formatContext->streams[streamId]->metadata, _properties.unknownStreams.back().metadatas );
				break;
			}
			case AVMEDIA_TYPE_NB:
			{
				break;
			}
		}
	}

	seekAtFrame( 0 );

	return *this;
}

Properties InputFile::analyseFile( const std::string& filename, IProgress& progress, const EAnalyseLevel level )
{
	InputFile file( filename );
	file.analyse( progress, level );
	Properties properties;
	file.getProperties( properties );
	return properties;
}

AVMediaType InputFile::getStreamType( size_t index )
{
	if( index >= _formatContext->nb_streams )
		return AVMEDIA_TYPE_UNKNOWN;
	return _formatContext->streams[index]->codec->codec_type;
}

AvInputStream& InputFile::getStream( size_t index )
{
	return *_inputStreams.at( index );
}

bool InputFile::readNextPacket( const size_t streamIndex )
{
	AVPacket packet;
	av_init_packet( &packet );
	while( 1 )
	{
		int ret = av_read_frame( _formatContext, &packet );
		if( ret < 0 ) // error or end of file
		{
			av_free_packet( &packet );
			return false;
		}

		// send packet to stream buffer
		_inputStreams.at( packet.stream_index )->addPacket( packet );

		// We only read one stream and skip others
		if( packet.stream_index == (int)streamIndex )
		{
			av_free_packet( &packet );
			return true;
		}

		// do not delete these 2 lines
		// need to skip packet, delete this one and re-init for reading the next one
		av_free_packet( &packet );
		av_init_packet( &packet );
	}
}

void InputFile::seekAtFrame( const size_t frame )
{
	uint64_t pos = frame / 25 * AV_TIME_BASE;  // WARNING: hardcoded fps

	if( (int)_formatContext->start_time != AV_NOPTS_VALUE )
		pos += _formatContext->start_time;

	if( av_seek_frame( _formatContext, -1, pos, AVSEEK_FLAG_BACKWARD ) < 0 )
	{
		std::cerr << "Error during seek at " << frame << " (" << pos << ") in file" << std::endl;
	}

	for( std::vector<AvInputStream*>::iterator it = _inputStreams.begin(); it != _inputStreams.end(); ++it )
	{
		(*it)->clearBuffering();
	}
}

void InputFile::readStream( const size_t streamIndex, bool readStream )
{
	_inputStreams.at( streamIndex )->setBufferred( readStream );
}

bool InputFile::getReadStream( const size_t streamIndex )
{
	return _inputStreams.at( streamIndex )->getBufferred();
}

void InputFile::setProfile( const Profile::ProfileDesc& desc )
{	
	ParamSet paramSet( _formatContext );
	
	for( Profile::ProfileDesc::const_iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == Profile::avProfileIdentificator ||
			(*it).first == Profile::avProfileIdentificatorHuman ||
			(*it).first == Profile::avProfileType )
			continue;
		
		try
		{
			paramSet.set( (*it).first, (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[InputFile] warning: " << e.what() << std::endl;
		}
	}
}

}
