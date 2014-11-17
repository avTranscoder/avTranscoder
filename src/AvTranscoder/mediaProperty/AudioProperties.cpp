#include "AudioProperties.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
}

namespace avtranscoder
{

AudioProperties::AudioProperties( const AVFormatContext* formatContext, const size_t index )
	: _formatContext( formatContext )
	, _codecContext( formatContext->streams[index]->codec )
	, _codec( NULL )
	, _streamId( index )
{
	if( _formatContext && _codecContext )
		_codec = avcodec_find_decoder( _codecContext->codec_id );
}

std::string AudioProperties::getCodecName() const
{
	if( _codec && _codec->name )
		return std::string( _codec->name );
	else
		return "unknown codec";
}

std::string AudioProperties::getCodecLongName() const
{
	if( _codec && _codec->long_name )
		return std::string( _codec->long_name );
	return "unknown codec";
}

std::string AudioProperties::getSampleFormatName() const
{
	const char* fmtName = av_get_sample_fmt_name( _codecContext->sample_fmt );
	if( fmtName )
		return std::string( fmtName );
	return "unknown sample format";
}

std::string AudioProperties::getSampleFormatLongName() const
{
	switch( _codecContext->sample_fmt )
	{
		case AV_SAMPLE_FMT_NONE:
			return "none";
		case AV_SAMPLE_FMT_U8:
			return "unsigned 8 bits";
		case AV_SAMPLE_FMT_S16:
			return "signed 16 bits";
		case AV_SAMPLE_FMT_S32:
			return "signed 32 bits";
		case AV_SAMPLE_FMT_FLT:
			return "float";
		case AV_SAMPLE_FMT_DBL:
			return "double";
		case AV_SAMPLE_FMT_U8P:
			return "unsigned 8 bits, planar";
		case AV_SAMPLE_FMT_S16P:
			return "signed 16 bits, planar";
		case AV_SAMPLE_FMT_S32P:
			return "signed 32 bits, planar";
		case AV_SAMPLE_FMT_FLTP:
			return "float, planar";
		case AV_SAMPLE_FMT_DBLP:
			return "double, planar";
		case AV_SAMPLE_FMT_NB:
			return "Number of sample formats.";
	}
	return "unknown sample format";
}

std::string AudioProperties::getChannelLayout() const
{
	char buf1[1024];
	av_get_channel_layout_string( buf1, sizeof( buf1 ), -1, _codecContext->channel_layout );
	return std::string( buf1 );
}

std::string AudioProperties::getChannelName() const
{
	const char* channelName = av_get_channel_name( _codecContext->channel_layout );
	if( channelName )
		return std::string( channelName );
	return "unknown channel name";
}

std::string AudioProperties::getChannelDescription() const
{
#ifdef FF_RESAMPLE_LIBRARY
	const char* channelDescription = av_get_channel_description( _codecContext->channel_layout );
	if( channelDescription )
		return std::string( channelDescription );
	return "unknown channel description";
#endif
	return "can't access channel description";
}

MetadatasMap AudioProperties::getDataMap() const
{
	MetadatasMap dataMap;

	detail::add( dataMap, "stream id", getStreamId() );
	detail::add( dataMap, "codec id", getCodecId() );
	detail::add( dataMap, "codec name", getCodecName() );
	detail::add( dataMap, "codec long name", getCodecLongName() );
	detail::add( dataMap, "sample format name", getSampleFormatName() );
	detail::add( dataMap, "sample format long name", getSampleFormatLongName() );
	detail::add( dataMap, "sample rate", getSampleRate() );
	detail::add( dataMap, "bit rate", getBitRate() );
	detail::add( dataMap, "channels", getChannels() );
	detail::add( dataMap, "channel layout", getChannelLayout() );
	detail::add( dataMap, "channel name", getChannelName() );
	detail::add( dataMap, "channel description", getChannelDescription() );

	for( size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, _metadatas.at( metadataIndex ).first, _metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

}
