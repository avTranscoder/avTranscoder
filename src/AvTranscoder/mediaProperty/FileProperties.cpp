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
	detail::add( dataMap, "format name", getFormatName() );
	detail::add( dataMap, "format long name", getFormatLongName() );

	detail::add( dataMap, "start time", getStartTime() );
	detail::add( dataMap, "duration", getDuration() );
	detail::add( dataMap, "bitrate", getBitRate() );
	detail::add( dataMap, "number of streams", getNbStreams() );
	detail::add( dataMap, "number of programs", getProgramsCount() );
	detail::add( dataMap, "number of video streams", getNbVideoStreams() );
	detail::add( dataMap, "number of audio streams", getNbAudioStreams() );
	detail::add( dataMap, "number of data streams", getNbDataStreams() );
	detail::add( dataMap, "number of subtitle streams", getNbSubtitleStreams() );
	detail::add( dataMap, "number of attachement streams", getNbAttachementStreams() );
	detail::add( dataMap, "number of unknown streams", getNbUnknownStreams() );

	for( size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, _metadatas.at( metadataIndex ).first, _metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

}
