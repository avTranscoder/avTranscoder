#include "InputFile.hpp"

#include <AvTranscoder/option/Context.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/mediaProperty/VideoProperties.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>
#include <AvTranscoder/mediaProperty/DataProperties.hpp>
#include <AvTranscoder/mediaProperty/SubtitleProperties.hpp>
#include <AvTranscoder/mediaProperty/AttachementProperties.hpp>
#include <AvTranscoder/mediaProperty/UnknownProperties.hpp>

extern "C" {
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
	, _properties( NULL )
	, _filename( filename )
	, _verbose( false )
{
	av_register_all();
	if( avformat_open_input( &_formatContext, _filename.c_str(), NULL, NULL ) < 0 )
	{
		std::string msg = "unable to open file: ";
		msg += _filename;
		throw std::ios_base::failure( msg );
	}

	// update format context informations from streams
	if( avformat_find_stream_info( _formatContext, NULL ) < 0 )
	{
		avformat_close_input( &_formatContext );
		_formatContext = NULL;
		throw std::ios_base::failure( "unable to find stream informations" );
	}

	// Initialize FileProperties
	_properties = FileProperties( _formatContext );

	// Create streams
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

void InputFile::analyse( IProgress& progress, const EAnalyseLevel level )
{
	assert( _formatContext != NULL );

	seekAtFrame( 0 );

	for( size_t streamId = 0; streamId < _formatContext->nb_streams; streamId++ )
	{
		switch( _formatContext->streams[streamId]->codec->codec_type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				VideoProperties properties( _formatContext, streamId, progress, level );
				_properties.getVideoProperties().push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				AudioProperties properties( _formatContext, streamId );
				_properties.getAudioProperties().push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_DATA:
			{
				DataProperties properties( _formatContext, streamId );
				_properties.getDataProperties().push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_SUBTITLE:
			{
				SubtitleProperties properties( _formatContext, streamId );
				_properties.getSubtitleProperties().push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_ATTACHMENT:
			{
				AttachementProperties properties( _formatContext, streamId );
				_properties.getAttachementProperties().push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_UNKNOWN:
			{
				UnknownProperties properties( _formatContext, streamId );
				_properties.getUnknownPropertiesProperties().push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_NB:
			{
				break;
			}
		}
	}

	seekAtFrame( 0 );
}

FileProperties InputFile::analyseFile( const std::string& filename, IProgress& progress, const EAnalyseLevel level )
{
	InputFile file( filename );
	file.analyse( progress, level );
	return file.getProperties();
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

bool InputFile::readNextPacket( CodedData& data, const size_t streamIndex )
{
	AVPacket packet;
	bool nextPacketFound = false;
	while( ! nextPacketFound )
	{
		av_init_packet( &packet );
		int ret = av_read_frame( _formatContext, &packet );
		if( ret < 0 ) // error or end of file
		{
			av_free_packet( &packet );
			return false;
		}

		// if the packet stream is the expected one
		// copy and return the packet data
		if( packet.stream_index == (int)streamIndex )
		{
			data.copyData( packet.data, packet.size );
			nextPacketFound = true;
		}
		// else add the packet data to the stream cache
		else
		{
			_inputStreams.at( packet.stream_index )->addPacket( packet );
		}
		av_free_packet( &packet );
	}
	return true;
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

void InputFile::activateStream( const size_t streamIndex, bool activate )
{
	_inputStreams.at( streamIndex )->activate( activate );
}

bool InputFile::isStreamActivated( const size_t streamIndex )
{
	return _inputStreams.at( streamIndex )->isActivated();
}

void InputFile::setProfile( const ProfileLoader::Profile& profile )
{	
	Context formatContext( _formatContext, AV_OPT_FLAG_DECODING_PARAM );
	
	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType )
			continue;
		
		try
		{
			Option& formatOption = formatContext.getOption( (*it).first );
			formatOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			if( _verbose )
				std::cout << "[InputFile] warning: " << e.what() << std::endl;
		}
	}
}

}