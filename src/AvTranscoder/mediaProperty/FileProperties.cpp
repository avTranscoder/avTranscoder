#include "FileProperties.hpp"

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


MetadatasMap FileProperties::getDataMap() const
{
	MetadatasMap dataMap;

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
