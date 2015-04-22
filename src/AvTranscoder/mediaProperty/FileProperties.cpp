#include "FileProperties.hpp"

#include <AvTranscoder/progress/NoDisplayProgress.hpp>

#include <stdexcept>
#include <sstream>

namespace avtranscoder
{

FileProperties::FileProperties( const FormatContext& formatContext )
	: _formatContext( &formatContext )
	, _avFormatContext( &formatContext.getAVFormatContext() )
	, _videoStreams()
	, _audioStreams()
	, _dataStreams()
	, _subtitleStreams()
	, _attachementStreams()
	, _unknownStreams()
{
	if( _avFormatContext )
		detail::fillMetadataDictionnary( _avFormatContext->metadata, _metadatas );

	NoDisplayProgress progress;
	extractStreamProperties( progress, eAnalyseLevelHeader );
}

void FileProperties::extractStreamProperties( IProgress& progress, const EAnalyseLevel level )
{
	clearStreamProperties();

	// if the analysis level wiil decode some streams parts, seek at the beginning
	if( level > eAnalyseLevelHeader )
		const_cast<FormatContext*>( _formatContext )->seek( 0 );

	for( size_t streamIndex = 0; streamIndex < _formatContext->getNbStreams(); ++streamIndex )
	{
		switch( _formatContext->getAVStream( streamIndex ).codec->codec_type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				VideoProperties properties( *_formatContext, streamIndex, progress, level );
				_videoStreams.push_back( properties );
				_streams.push_back( &_videoStreams.back() );
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				AudioProperties properties( *_formatContext, streamIndex );
				_audioStreams.push_back( properties );
				_streams.push_back( &_audioStreams.back() );
				break;
			}
			case AVMEDIA_TYPE_DATA:
			{
				DataProperties properties( *_formatContext, streamIndex );
				_dataStreams.push_back( properties );
				_streams.push_back( &_dataStreams.back() );
				break;
			}
			case AVMEDIA_TYPE_SUBTITLE:
			{
				SubtitleProperties properties( *_formatContext, streamIndex );
				_subtitleStreams.push_back( properties );
				_streams.push_back( &_subtitleStreams.back() );
				break;
			}
			case AVMEDIA_TYPE_ATTACHMENT:
			{
				AttachementProperties properties( *_formatContext, streamIndex );
				_attachementStreams.push_back( properties );
				_streams.push_back( &_attachementStreams.back() );
				break;
			}
			case AVMEDIA_TYPE_UNKNOWN:
			{
				UnknownProperties properties( *_formatContext, streamIndex );
				_unknownStreams.push_back( properties );
				_streams.push_back( &_unknownStreams.back() );
				break;
			}
			case AVMEDIA_TYPE_NB:
			{
				break;
			}
		}
	}

	// if the analysis level has decoded some streams parts, return at the beginning
	if( level > eAnalyseLevelHeader )
		const_cast<FormatContext*>( _formatContext )->seek( 0 );
}

std::string FileProperties::getFilename() const
{
	if( ! _avFormatContext || ! _avFormatContext->filename )
		throw std::runtime_error( "unknown file name" );
	return _avFormatContext->filename;
}

std::string FileProperties::getFormatName() const
{
	if( ! _avFormatContext || ! _avFormatContext->iformat || ! _avFormatContext->iformat->name )
		throw std::runtime_error( "unknown format name");
	return _avFormatContext->iformat->name;
}

std::string FileProperties::getFormatLongName() const
{
	if( ! _avFormatContext || ! _avFormatContext->iformat || ! _avFormatContext->iformat->long_name )
		throw std::runtime_error( "unknown format long name");
	return _avFormatContext->iformat->long_name;
}

size_t FileProperties::getProgramsCount() const
{
	if( ! _avFormatContext )
		throw std::runtime_error( "unknown format context" );
	return _avFormatContext->nb_programs;
}

double FileProperties::getStartTime() const
{
	if( ! _avFormatContext )
		throw std::runtime_error( "unknown format context" );
	return  1.0 * (unsigned int)_avFormatContext->start_time / AV_TIME_BASE;
}

double FileProperties::getDuration() const
{
	if( ! _avFormatContext )
		throw std::runtime_error( "unknown format context" );
	return 1.0 * _avFormatContext->duration / AV_TIME_BASE;
}

size_t FileProperties::getBitRate() const
{
	if( ! _avFormatContext )
		throw std::runtime_error( "unknown format context" );
	return _avFormatContext->bit_rate;
}

size_t FileProperties::getPacketSize() const
{
	if( ! _avFormatContext )
		throw std::runtime_error( "unknown format context" );
	return _avFormatContext->packet_size;
}

const avtranscoder::StreamProperties& FileProperties::getStreamPropertiesWithIndex( const size_t streamIndex ) const
{
	for( std::vector< StreamProperties* >::const_iterator it = _streams.begin(); it != _streams.end(); ++it )
	{
		if( (*it)->getStreamIndex() == streamIndex )
			return *(*it);
	}
	std::stringstream os;
	os << "No stream properties correspond to stream at index ";
	os <<  streamIndex;
	throw std::runtime_error( os.str() );
}

size_t FileProperties::getNbStreams() const
{
	if( ! _avFormatContext )
		throw std::runtime_error( "unknown format context" );
	return _avFormatContext->nb_streams;
}

PropertyVector FileProperties::getPropertiesAsVector() const
{
	PropertyVector data;

	addProperty( data, "filename", &FileProperties::getFilename );
	addProperty( data, "formatName", &FileProperties::getFormatName );
	addProperty( data, "formatLongName", &FileProperties::getFormatLongName );

	addProperty( data, "startTime", &FileProperties::getStartTime );
	addProperty( data, "duration", &FileProperties::getDuration );
	addProperty( data, "bitrate", &FileProperties::getBitRate );
	addProperty( data, "numberOfStreams", &FileProperties::getNbStreams );
	addProperty( data, "numberOfPrograms", &FileProperties::getProgramsCount );

	detail::add( data, "numberOfVideoStreams", getNbVideoStreams() );
	detail::add( data, "numberOfAudioStreams", getNbAudioStreams() );
	detail::add( data, "numberOfDataStreams", getNbDataStreams() );
	detail::add( data, "numberOfSubtitleStreams", getNbSubtitleStreams() );
	detail::add( data, "numberOfAttachementStreams", getNbAttachementStreams() );
	detail::add( data, "numberOfUnknownStreams", getNbUnknownStreams() );

	for( size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex )
	{
		detail::add( data, _metadatas.at( metadataIndex ).first, _metadatas.at( metadataIndex ).second );
	}

	return data;
}

void FileProperties::clearStreamProperties()
{
	_streams.clear();

	_videoStreams.clear();
	_audioStreams.clear();
	_dataStreams.clear();
	_subtitleStreams.clear();
	_attachementStreams.clear();
	_unknownStreams.clear();
}

}
