#ifndef _AV_TRANSCODER_AUDIO_STREAM_PROPERTIES_HPP_
#define _AV_TRANSCODER_AUDIO_STREAM_PROPERTIES_HPP_

//#include <AvTranscoder/mediaProperty/MediaMetadatasStructures.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
}

namespace avtranscoder
{

avtranscoder::AudioProperties audioStreamInfo( const AVFormatContext* formatContext, const size_t index )
{
	AudioProperties ap;
	AVCodecContext* codec_context = formatContext->streams[index]->codec;
	
	ap.codecId       = codec_context->codec_id;
	ap.sampleRate    = codec_context->sample_rate;
	ap.channels      = codec_context->channels;
	ap.bit_rate      = codec_context->bit_rate;
	ap.streamId      = index;

	AVCodec* codec = avcodec_find_decoder( codec_context->codec_id );
	if( codec != NULL )
	{
		ap.codecName = codec->name;
		ap.codecLongName = codec->long_name;
	}

	char buf1[1024];
	av_get_channel_layout_string( buf1, sizeof( buf1 ), -1, codec_context->channel_layout );
	
	ap.channelLayout = std::string( buf1 );

	const char* channelName = av_get_channel_name( codec_context->channel_layout );
	if( channelName )
		ap.channelName = std::string( channelName );
#ifdef FF_RESAMPLE_LIBRARY
	const char* channelDescription = av_get_channel_description( codec_context->channel_layout );
	if( channelDescription )
		ap.channelDescription = std::string( channelDescription );
#endif
	
	std::string sampleFormat = "";
	switch( codec_context->sample_fmt )
	{
		case AV_SAMPLE_FMT_NONE : ap.sampleFormat = "none"; break;
		case AV_SAMPLE_FMT_U8   : ap.sampleFormat = "unsigned 8 bits"; break;
		case AV_SAMPLE_FMT_S16  : ap.sampleFormat = "signed 16 bits"; break;
		case AV_SAMPLE_FMT_S32  : ap.sampleFormat = "signed 32 bits"; break;
		case AV_SAMPLE_FMT_FLT  : ap.sampleFormat = "float"; break;
		case AV_SAMPLE_FMT_DBL  : ap.sampleFormat = "double"; break;
		case AV_SAMPLE_FMT_U8P  : ap.sampleFormat = "unsigned 8 bits, planar"; break;
		case AV_SAMPLE_FMT_S16P : ap.sampleFormat = "signed 16 bits, planar"; break;
		case AV_SAMPLE_FMT_S32P : ap.sampleFormat = "signed 32 bits, planar"; break;
		case AV_SAMPLE_FMT_FLTP : ap.sampleFormat = "float, planar"; break;
		case AV_SAMPLE_FMT_DBLP : ap.sampleFormat = "double, planar"; break;
		case AV_SAMPLE_FMT_NB   : ap.sampleFormat = "Number of sample formats."; break;
	}

	return ap;
}

}

#endif
