#ifndef _AV_TRANSCODER_AUDIO_STREAM_PROPERTIES_HPP_
#define _AV_TRANSCODER_AUDIO_STREAM_PROPERTIES_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

namespace avtranscoder
{

AudioProperties audioStreamInfo( const AVFormatContext* formatContext, const size_t index )
{
	AudioProperties ap;
	AVCodecContext* codec_context = formatContext->streams[index]->codec;
	
	ap.codecName     = codec_context->codec_name;
	ap.codecLongName = codec_context->codec_name;
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