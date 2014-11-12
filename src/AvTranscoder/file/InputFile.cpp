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
{
	av_register_all();
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

InputFile& InputFile::analyse( IProgress& progress, const EAnalyseLevel level )
{
	assert( _formatContext != NULL );

	seekAtFrame( 0 );

	for( size_t streamId = 0; streamId < _formatContext->nb_streams; streamId++ )
	{
		switch( _formatContext->streams[streamId]->codec->codec_type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				_properties.getVideoProperties().push_back( VideoProperties( _formatContext, streamId, progress, level ) );
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				_properties.getAudioProperties().push_back( AudioProperties( _formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_DATA:
			{
				_properties.getDataProperties().push_back( DataProperties( _formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_SUBTITLE:
			{
				_properties.getSubtitleProperties().push_back( SubtitleProperties( _formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_ATTACHMENT:
			{
				_properties.getAttachementProperties().push_back( AttachementProperties( _formatContext, streamId ) );
				break;
			}
			case AVMEDIA_TYPE_UNKNOWN:
			{
				_properties.getUnknownPropertiesProperties().push_back( UnknownProperties( _formatContext, streamId ) );
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

void InputFile::setProfile( const ProfileLoader::Profile& profile )
{	
	Context formatContext( _formatContext );
	
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
			std::cout << "[InputFile] warning: " << e.what() << std::endl;
		}
	}
}

}