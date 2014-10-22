#include "mediaProperty.hpp"

extern "C" {
#include <libavutil/dict.h>
}

#include <sstream>
#include <utility>

namespace avtranscoder
{

namespace detail
{

template<typename T>
void add( MetadatasMap& dataMap, const std::string& key, const T& value )
{
	std::stringstream ss;
	ss << value;
	add( dataMap, key, ss.str() );
}

template<>
void add( MetadatasMap& dataMap, const std::string& key, const std::string& value )
{
	dataMap.push_back( std::pair<std::string, std::string>( key, value ) );
}

template<>
void add( MetadatasMap& dataMap, const std::string& key, const bool& value )
{
	add( dataMap, key, value ? "True" : "False" );
}

void fillMetadataDictionnary( AVDictionary* avdictionnary, MetadatasMap& metadata )
{
	AVDictionaryEntry* tag = NULL;
	while( ( tag = av_dict_get( avdictionnary, "", tag, AV_DICT_IGNORE_SUFFIX ) ) )
	{
		metadata.push_back( std::pair<std::string, std::string>( tag->key, tag->value ) );
	}
}

}

MetadatasMap VideoProperties::getDataMap() const
{
	MetadatasMap dataMap;

	detail::add( dataMap, "stream id", streamId );
	detail::add( dataMap, "codec id", codecId );
	detail::add( dataMap, "codec name", codecName );
	detail::add( dataMap, "codec long name", codecLongName );
	detail::add( dataMap, "profile", profile );
	detail::add( dataMap, "profile name", profileName );
	detail::add( dataMap, "level", level );
	detail::add( dataMap, "start timecode", startTimecode );
	detail::add( dataMap, "width", width );
	detail::add( dataMap, "height", height );
	detail::add( dataMap, "pixel aspect ratio", sar.num / sar.den );
	detail::add( dataMap, "display aspect ratio", dar.num / dar.den );
	detail::add( dataMap, "dtgActiveFormat", dtgActiveFormat );
	detail::add( dataMap, "components count", componentsCount );
	detail::add( dataMap, "pixel type", pixelName );
	detail::add( dataMap, "bit wise acked", bitWisePacked );
	detail::add( dataMap, "rgb pixel", rgbPixelData );
	detail::add( dataMap, "as alpha", asAlpha );
	detail::add( dataMap, "chroma width", chromaWidth );
	detail::add( dataMap, "chroma height", chromaHeight );
	detail::add( dataMap, "endianess", endianess );
	detail::add( dataMap, "color transfert", colorTransfert );
	detail::add( dataMap, "colorspace", colorspace );
	detail::add( dataMap, "color range", colorRange );
	detail::add( dataMap, "color primaries", colorPrimaries );
	detail::add( dataMap, "indexed colors", indexedColors );
	detail::add( dataMap, "pseudo paletted", pseudoPaletted );
	detail::add( dataMap, "chroma sample location", chromaSampleLocation);
	detail::add( dataMap, "interlaced ", isInterlaced );
	detail::add( dataMap, "top field first", topFieldFirst );
	detail::add( dataMap, "field order", fieldOrder);
	detail::add( dataMap, "timeBase", timeBase.num / timeBase.den );
	detail::add( dataMap, "fps", fps );
	detail::add( dataMap, "ticksPerFrame", ticksPerFrame );
	detail::add( dataMap, "bit rate", bitRate );
	detail::add( dataMap, "max bit rate", maxBitRate );
	detail::add( dataMap, "min bit rate", minBitRate );
	detail::add( dataMap, "gop size", gopSize );

	std::string gop;
	for( size_t frameIndex = 0; frameIndex < gopStructure.size(); ++frameIndex )
	{
		gop += gopStructure.at( frameIndex ).first;
		gop += ( gopStructure.at( frameIndex ).second ? "*" : " " );
	}
	detail::add( dataMap, "gop", gop );

	detail::add( dataMap, "has B frames", hasBFrames );
	detail::add( dataMap, "references frames", referencesFrames );

	for( size_t metadataIndex = 0; metadataIndex < metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, metadatas.at( metadataIndex ).first, metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

MetadatasMap AudioProperties::getDataMap() const
{
	MetadatasMap dataMap;

	detail::add( dataMap, "stream id", streamId );
	detail::add( dataMap, "codec id", codecId );
	detail::add( dataMap, "codec name", codecName );
	detail::add( dataMap, "codec long name", codecLongName );
	detail::add( dataMap, "sample format", sampleFormat );
	detail::add( dataMap, "sample rate", sampleRate );
	detail::add( dataMap, "bit rate", bit_rate );
	detail::add( dataMap, "channels", channels );
	detail::add( dataMap, "channel layout", channelLayout );
	detail::add( dataMap, "channel name", channelName );
	detail::add( dataMap, "channel description", channelDescription );

	for( size_t metadataIndex = 0; metadataIndex < metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, metadatas.at( metadataIndex ).first, metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

MetadatasMap DataProperties::getDataMap() const
{
	MetadatasMap dataMap;

	detail::add( dataMap, "streamId", streamId );

	for( size_t metadataIndex = 0; metadataIndex < metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, metadatas.at( metadataIndex ).first, metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

MetadatasMap SubtitleProperties::getDataMap() const
{
	MetadatasMap dataMap;

	detail::add( dataMap, "streamId", streamId );

	for( size_t metadataIndex = 0; metadataIndex < metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, metadatas.at( metadataIndex ).first, metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

MetadatasMap AttachementProperties::getDataMap() const
{
	MetadatasMap dataMap;

	detail::add( dataMap, "streamId", streamId );

	for( size_t metadataIndex = 0; metadataIndex < metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, metadatas.at( metadataIndex ).first, metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

MetadatasMap UnknownProperties::getDataMap() const
{
	MetadatasMap dataMap;

	detail::add( dataMap, "streamId", streamId );

	for( size_t metadataIndex = 0; metadataIndex < metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, metadatas.at( metadataIndex ).first, metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

MetadatasMap Properties::getDataMap() const
{
	MetadatasMap dataMap;

	detail::add( dataMap, "filename", filename );
	detail::add( dataMap, "format name", formatName );
	detail::add( dataMap, "format long name", formatLongName );

	detail::add( dataMap, "start time", startTime );
	detail::add( dataMap, "duration", duration );
	detail::add( dataMap, "bitrate", bitRate );
	detail::add( dataMap, "number of streams", streamsCount );
	detail::add( dataMap, "number of programs", programsCount );
	detail::add( dataMap, "number of video streams", videoStreams.size() );
	detail::add( dataMap, "number of audio streams", audioStreams.size() );
	detail::add( dataMap, "number of data streams", dataStreams.size() );
	detail::add( dataMap, "number of subtitle streams", subtitleStreams.size() );
	detail::add( dataMap, "number of attachement streams", attachementStreams.size() );
	detail::add( dataMap, "number of unknown streams", unknownStreams.size() );

	for( size_t metadataIndex = 0; metadataIndex < metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, metadatas.at( metadataIndex ).first, metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

}
