#include "FileProperties.hpp"

#include <stdexcept>

namespace avtranscoder
{

FileProperties::FileProperties( const FormatContext& formatContext )
	: _formatContext( &formatContext.getAVFormatContext() )
	, _videoStreams()
	, _audioStreams()
	, _dataStreams()
	, _subtitleStreams()
	, _attachementStreams()
	, _unknownStreams()
{
	if( _formatContext )
		detail::fillMetadataDictionnary( _formatContext->metadata, _metadatas );
}

std::string FileProperties::getFilename() const
{
	if( ! _formatContext || ! _formatContext->filename )
		throw std::runtime_error( "unknown file name" );
	return _formatContext->filename;
}

std::string FileProperties::getFormatName() const
{
	if( ! _formatContext || ! _formatContext->iformat || ! _formatContext->iformat->name )
		throw std::runtime_error( "unknown format name");
	return _formatContext->iformat->name;
}

std::string FileProperties::getFormatLongName() const
{
	if( ! _formatContext || ! _formatContext->iformat || ! _formatContext->iformat->long_name )
		throw std::runtime_error( "unknown format long name");
	return _formatContext->iformat->long_name;
}

size_t FileProperties::getProgramsCount() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->nb_programs;
}

double FileProperties::getStartTime() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return  1.0 * (unsigned int)_formatContext->start_time / AV_TIME_BASE;
}

double FileProperties::getDuration() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return 1.0 * _formatContext->duration / AV_TIME_BASE;
}

size_t FileProperties::getBitRate() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->bit_rate;
}

size_t FileProperties::getPacketSize() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->packet_size;
}

VideoProperties& FileProperties::getVideoPropertiesWithStreamIndex( const size_t streamIndex )
{
	for( std::vector< VideoProperties >::iterator it = _videoStreams.begin(); it != _videoStreams.end(); ++it )
	{
		if( it->getStreamIndex() == streamIndex )
			return *it;
	}
	std::string msg( "no video properties correspond to stream at index " );
	msg += streamIndex;
	throw std::runtime_error( msg );
}

const avtranscoder::VideoProperties& FileProperties::getVideoPropertiesWithStreamIndex( const size_t streamIndex ) const
{
	for( std::vector< VideoProperties >::const_iterator it = _videoStreams.begin(); it != _videoStreams.end(); ++it )
	{
		if( it->getStreamIndex() == streamIndex )
			return *it;
	}
	std::string msg( "no video properties correspond to stream at index " );
	msg += streamIndex;
	throw std::runtime_error( msg );
}

AudioProperties& FileProperties::getAudioPropertiesWithStreamIndex( const size_t streamIndex )
{
	for( std::vector< AudioProperties >::iterator it = _audioStreams.begin(); it != _audioStreams.end(); ++it )
	{
		if( it->getStreamIndex() == streamIndex )
			return *it;
	}
	std::string msg( "no audio properties correspond to stream at index " );
	msg += streamIndex;
	throw std::runtime_error( msg );
}

const avtranscoder::AudioProperties& FileProperties::getAudioPropertiesWithStreamIndex( const size_t streamIndex ) const
{
	for( std::vector< AudioProperties >::const_iterator it = _audioStreams.begin(); it != _audioStreams.end(); ++it )
	{
		if( it->getStreamIndex() == streamIndex )
			return *it;
	}
	std::string msg( "no audio properties correspond to stream at index " );
	msg += streamIndex;
	throw std::runtime_error( msg );
}

size_t FileProperties::getNbStreams() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->nb_streams;
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
	_videoStreams.clear();
	_audioStreams.clear();
	_dataStreams.clear();
	_subtitleStreams.clear();
	_attachementStreams.clear();
	_unknownStreams.clear();
}

}
