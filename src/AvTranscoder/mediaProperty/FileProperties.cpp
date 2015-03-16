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
		return "unknown file name";
	return _formatContext->filename;
}

std::string FileProperties::getFormatName() const
{
	if( ! _formatContext || ! _formatContext->iformat || ! _formatContext->iformat->name )
		return "unknown format name";
	return _formatContext->iformat->name;
}

std::string FileProperties::getFormatLongName() const
{
	if( ! _formatContext || ! _formatContext->iformat || ! _formatContext->iformat->long_name )
		return "unknown format long name";
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

PropertiesMap FileProperties::getPropertiesAsMap() const
{
	PropertiesMap dataMap;

	addProperty( dataMap, "filename", &FileProperties::getFilename );
	addProperty( dataMap, "formatName", &FileProperties::getFormatName );
	addProperty( dataMap, "formatLongName", &FileProperties::getFormatLongName );

	addProperty( dataMap, "startTime", &FileProperties::getStartTime );
	addProperty( dataMap, "duration", &FileProperties::getDuration );
	addProperty( dataMap, "bitrate", &FileProperties::getBitRate );
	addProperty( dataMap, "numberOfStreams", &FileProperties::getNbStreams );
	addProperty( dataMap, "numberOfPrograms", &FileProperties::getProgramsCount );

	detail::add( dataMap, "numberOfVideoStreams", getNbVideoStreams() );
	detail::add( dataMap, "numberOfAudioStreams", getNbAudioStreams() );
	detail::add( dataMap, "numberOfDataStreams", getNbDataStreams() );
	detail::add( dataMap, "numberOfSubtitleStreams", getNbSubtitleStreams() );
	detail::add( dataMap, "numberOfAttachementStreams", getNbAttachementStreams() );
	detail::add( dataMap, "numberOfUnknownStreams", getNbUnknownStreams() );

	for( size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, _metadatas.at( metadataIndex ).first, _metadatas.at( metadataIndex ).second );
	}

	return dataMap;
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
