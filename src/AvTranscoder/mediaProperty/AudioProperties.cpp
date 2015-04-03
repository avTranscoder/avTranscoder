#include "AudioProperties.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
}

#include <stdexcept>

namespace avtranscoder
{

AudioProperties::AudioProperties( const FormatContext& formatContext, const size_t index )
	: StreamProperties( formatContext, index )
{
	if( _formatContext )
		_codecContext = _formatContext->streams[index]->codec;

	if( _formatContext && _codecContext )
		_codec = avcodec_find_decoder( _codecContext->codec_id );

	if( _formatContext )
		detail::fillMetadataDictionnary( _formatContext->streams[index]->metadata, _metadatas );
}

size_t AudioProperties::getStreamId() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->streams[_streamIndex]->id;
}

std::string AudioProperties::getCodecName() const
{
	if( ! _codec || ! _codec->name )
		throw std::runtime_error( "unknown codec name" );
	return std::string( _codec->name );
}

std::string AudioProperties::getCodecLongName() const
{
	if( ! _codec || ! _codec->long_name )
		throw std::runtime_error( "unknown codec long name" );
	return std::string( _codec->long_name );
}

std::string AudioProperties::getSampleFormatName() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

	const char* fmtName = av_get_sample_fmt_name( _codecContext->sample_fmt );
	if( ! fmtName )
		throw std::runtime_error( "unknown sample format" );

	return std::string( fmtName );
}

std::string AudioProperties::getSampleFormatLongName() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

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
			return "number of sample formats";
	}
	return "unknown sample format";
}

std::string AudioProperties::getChannelLayout() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

	char buf1[1024];
	av_get_channel_layout_string( buf1, sizeof( buf1 ), -1, _codecContext->channel_layout );
	return std::string( buf1 );
}

std::string AudioProperties::getChannelName() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

	const char* channelName = av_get_channel_name( _codecContext->channel_layout );
	if( ! channelName )
		throw std::runtime_error( "unknown channel name" );

	return std::string( channelName );
}

std::string AudioProperties::getChannelDescription() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

#ifdef AVTRANSCODER_FFMPEG_DEPENDENCY
	const char* channelDescription = av_get_channel_description( _codecContext->channel_layout );
	if( ! channelDescription )
		throw std::runtime_error( "unknown channel description" );
	return std::string( channelDescription );
#else
	throw std::runtime_error( "can't access channel description" );
#endif
}

size_t AudioProperties::getCodecId() const 
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->codec_id;
}

size_t AudioProperties::getSampleRate() const 
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->sample_rate;
}

size_t AudioProperties::getChannels() const 
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->channels;
}

size_t AudioProperties::getBitRate() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

	// return bit rate of stream
	if( _codecContext->bit_rate )
		return _codecContext->bit_rate;

	// else get computed bit rate from our computation (warning: way to compute bit rate of PCM audio data)
	int bitsPerSample = av_get_bits_per_sample( _codecContext->codec_id ); // 0 if unknown for the given codec
	return _codecContext->sample_rate * _codecContext->channels * bitsPerSample;
}

size_t AudioProperties::getNbSamples() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	return _formatContext->streams[_streamIndex]->nb_frames;
}

size_t AudioProperties::getTicksPerFrame() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->ticks_per_frame;
}

Rational AudioProperties::getTimeBase() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );

	Rational timeBase = {
		_formatContext->streams[_streamIndex]->time_base.num,
		_formatContext->streams[_streamIndex]->time_base.den,
	};
	return timeBase;
}

double AudioProperties::getDuration() const
{
	Rational timeBase = getTimeBase();
	double duration = ( timeBase.num / (double) timeBase.den ) * _formatContext->streams[_streamIndex]->duration;
	return duration;
}

PropertyVector AudioProperties::getPropertiesAsVector() const
{
	PropertyVector data;

	addProperty( data, "streamId", &AudioProperties::getStreamId );
	addProperty( data, "codecId", &AudioProperties::getCodecId );
	addProperty( data, "codecName", &AudioProperties::getCodecName );
	addProperty( data, "codecLongName", &AudioProperties::getCodecLongName );
	addProperty( data, "sampleFormatName", &AudioProperties::getSampleFormatName );
	addProperty( data, "sampleFormatLongName", &AudioProperties::getSampleFormatLongName );
	addProperty( data, "sampleRate", &AudioProperties::getSampleRate );
	addProperty( data, "bitRate", &AudioProperties::getBitRate );
	addProperty( data, "nbSamples", &AudioProperties::getNbSamples );
	addProperty( data, "channels", &AudioProperties::getChannels );
	addProperty( data, "channelLayout", &AudioProperties::getChannelLayout );
	addProperty( data, "channelName", &AudioProperties::getChannelName );
	addProperty( data, "channelDescription", &AudioProperties::getChannelDescription );
	addProperty( data, "ticksPerFrame", &AudioProperties::getTicksPerFrame );
	addProperty( data, "timeBase", &AudioProperties::getTimeBase );
	addProperty( data, "duration", &AudioProperties::getDuration );

	for( size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex )
	{
		detail::add( data, _metadatas.at( metadataIndex ).first, _metadatas.at( metadataIndex ).second );
	}

	return data;
}

}
