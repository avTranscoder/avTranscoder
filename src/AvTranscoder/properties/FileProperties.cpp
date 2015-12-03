#include "FileProperties.hpp"

#include <AvTranscoder/properties/JsonWriter.hpp>
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
		const_cast<FormatContext*>( _formatContext )->seek( 0, AVSEEK_FLAG_BACKWARD );

	for( size_t streamIndex = 0; streamIndex < _formatContext->getNbStreams(); ++streamIndex )
	{
		switch( _formatContext->getAVStream( streamIndex ).codec->codec_type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				VideoProperties properties( *_formatContext, streamIndex, progress, level );
				_videoStreams.push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				AudioProperties properties( *_formatContext, streamIndex );
				_audioStreams.push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_DATA:
			{
				DataProperties properties( *_formatContext, streamIndex );
				_dataStreams.push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_SUBTITLE:
			{
				SubtitleProperties properties( *_formatContext, streamIndex );
				_subtitleStreams.push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_ATTACHMENT:
			{
				AttachementProperties properties( *_formatContext, streamIndex );
				_attachementStreams.push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_UNKNOWN:
			{
				UnknownProperties properties( *_formatContext, streamIndex );
				_unknownStreams.push_back( properties );
				break;
			}
			case AVMEDIA_TYPE_NB:
			{
				break;
			}
		}
	}

	// once the streams vectors are filled, add their references the base streams vector
	for( size_t streamIndex = 0; streamIndex < _videoStreams.size(); ++streamIndex )
	{
		const size_t videoStreamIndex = _videoStreams.at( streamIndex ).getStreamIndex();
		_streams[ videoStreamIndex ] = &_videoStreams.at( streamIndex );
	}

	for( size_t streamIndex = 0; streamIndex < _audioStreams.size(); ++ streamIndex )
	{
		const size_t audioStreamIndex = _audioStreams.at( streamIndex ).getStreamIndex();
		_streams[ audioStreamIndex ] = &_audioStreams.at(streamIndex);
	}

	for( size_t streamIndex = 0; streamIndex < _dataStreams.size(); ++ streamIndex )
	{
		const size_t dataStreamIndex = _dataStreams.at( streamIndex ).getStreamIndex();
		_streams[ dataStreamIndex ] = &_dataStreams.at(streamIndex);
	}

	for( size_t streamIndex = 0; streamIndex < _subtitleStreams.size(); ++ streamIndex )
	{
		const size_t subtitleStreamIndex = _subtitleStreams.at( streamIndex ).getStreamIndex();
		_streams[ subtitleStreamIndex ] = &_subtitleStreams.at(streamIndex);
	}

	for( size_t streamIndex = 0; streamIndex < _attachementStreams.size(); ++ streamIndex )
	{
		const size_t attachementStreamIndex = _attachementStreams.at( streamIndex ).getStreamIndex();
		_streams[ attachementStreamIndex ] = &_attachementStreams.at(streamIndex);
	}

	for( size_t streamIndex = 0; streamIndex < _unknownStreams.size(); ++ streamIndex )
	{
		const size_t unknownStreamIndex = _unknownStreams.at( streamIndex ).getStreamIndex();
		_streams[ unknownStreamIndex ] = &_unknownStreams.at(streamIndex);
	}

	// if the analysis level has decoded some streams parts, return at the beginning
	if( level > eAnalyseLevelHeader )
		const_cast<FormatContext*>( _formatContext )->seek( 0, AVSEEK_FLAG_BACKWARD );
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

float FileProperties::getDuration() const
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
	avtranscoder::StreamProperties* properties = _streams.find( streamIndex )->second;
	if( properties )
		return *properties;
	std::stringstream os;
	os << "No stream properties correspond to stream at index ";
	os <<  streamIndex;
	throw std::runtime_error( os.str() );
}

const std::vector< avtranscoder::StreamProperties* > FileProperties::getStreamProperties() const
{
	std::vector< avtranscoder::StreamProperties* > streams;
	for( std::map< size_t, StreamProperties* >::const_iterator it = _streams.begin(); it != _streams.end(); ++it )
	{
		streams.push_back( it->second );
	}
	return streams;
}

size_t FileProperties::getNbStreams() const
{
	if( ! _avFormatContext )
		throw std::runtime_error( "unknown format context" );
	return _avFormatContext->nb_streams;
}

PropertyVector FileProperties::asVector() const
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

PropertyMap FileProperties::asMap() const
{
	PropertyMap dataMap;

	PropertyVector dataVector( asVector() );
	for( PropertyVector::const_iterator it = dataVector.begin();
			it != dataVector.end();
			++it )
	{
		dataMap.insert( std::make_pair( it->first, it->second ) );
	}

	return dataMap;
}

std::string FileProperties::asJson() const
{
	json::JsonObjectStreamWriter writer;
	PropertyMap properties = asMap();
	for(PropertyMap::iterator it = properties.begin(); it != properties.end(); ++it)
		writer << std::make_pair(it->first.c_str(), it->second.c_str());
	return writer.build();
}

std::string FileProperties::allPropertiesAsJson() const
{
	json::JsonObjectStreamWriter writer;
	{
		// format
		json::JsonArrayStreamWriter format;
		format << asJson();
		writer << std::make_pair("format", format.build());
	}
	{
		// video streams
		json::JsonArrayStreamWriter video;
		for( std::vector< avtranscoder::VideoProperties >::const_iterator it = _videoStreams.begin(); it != _videoStreams.end(); ++it )
		{
			video << it->asJson();
		}
		writer << std::make_pair("video", video.build());
	}
	{
		// audio streams
		json::JsonArrayStreamWriter audio;
		for( std::vector< avtranscoder::AudioProperties >::const_iterator it = _audioStreams.begin(); it != _audioStreams.end(); ++it )
		{
			audio << it->asJson();
		}
		writer << std::make_pair("audio", audio.build());
	}
	{
		// data streams
		json::JsonArrayStreamWriter data;
		for( std::vector< avtranscoder::DataProperties >::const_iterator it = _dataStreams.begin(); it != _dataStreams.end(); ++it )
		{
			data << it->asJson();
		}
		writer << std::make_pair("data", data.build());
	}
	{
		// subtitle streams
		json::JsonArrayStreamWriter subtitle;
		for( std::vector< avtranscoder::SubtitleProperties >::const_iterator it = _subtitleStreams.begin(); it != _subtitleStreams.end(); ++it )
		{
			subtitle << it->asJson();
		}
		writer << std::make_pair("subtitle", subtitle.build());
	}
	{
		// attachement streams
		json::JsonArrayStreamWriter attachement;
		for( std::vector< avtranscoder::AttachementProperties >::const_iterator it = _attachementStreams.begin(); it != _attachementStreams.end(); ++it )
		{
			attachement << it->asJson();
		}
		writer << std::make_pair("attachement", attachement.build());
	}
	{
		// unknown streams
		json::JsonArrayStreamWriter unknown;
		for( std::vector< avtranscoder::UnknownProperties >::const_iterator it = _unknownStreams.begin(); it != _unknownStreams.end(); ++it )
		{
			unknown << it->asJson();
		}
		writer << std::make_pair("unknown", unknown.build());
	}
	return writer.build();
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
