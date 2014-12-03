#include "FileProperties.hpp"

#include <stdexcept>

namespace avtranscoder
{

FileProperties::FileProperties( const AVFormatContext* formatContext )
	: _formatContext( formatContext )
	, _videoStreams()
	, _audioStreams()
	, _dataStreams()
	, _subtitleStreams()
	, _attachementStreams()
	, _unknownStreams()
{
	if( formatContext )
		detail::fillMetadataDictionnary( _formatContext->metadata, _metadatas );
}

std::string FileProperties::getFilename() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->filename;
}

std::string FileProperties::getFormatName() const
{
	if( ! _formatContext || ! _formatContext->iformat )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->iformat->name;
}

std::string FileProperties::getFormatLongName() const
{
	if( ! _formatContext || ! _formatContext->iformat )
		throw std::runtime_error( "unknown format context" );
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

size_t FileProperties::getNbStreams() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->nb_streams;
}

PropertiesMap FileProperties::getDataMap() const
{
	PropertiesMap dataMap;

	detail::add( dataMap, "filename", getFilename() );
	detail::add( dataMap, "formatName", getFormatName() );
	detail::add( dataMap, "formatLongName", getFormatLongName() );

	detail::add( dataMap, "startTime", getStartTime() );
	detail::add( dataMap, "duration", getDuration() );
	detail::add( dataMap, "bitrate", getBitRate() );
	detail::add( dataMap, "numberOfStreams", getNbStreams() );
	detail::add( dataMap, "numberOfPrograms", getProgramsCount() );
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

}
