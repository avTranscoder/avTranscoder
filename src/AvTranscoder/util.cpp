#include "util.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
}

namespace avtranscoder
{

std::vector<std::string> getPixelFormats( const std::string& videoCodecName )
{
	std::vector<std::string> pixelFormats;
	
	// all video codec concerned
	if( videoCodecName == "" )
	{
		const AVPixFmtDescriptor* pixFmtDesc = NULL; 

#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT( 51, 44, 0 )
		for( int pix_fmt = 0; pix_fmt < PIX_FMT_NB; ++pix_fmt )
			pixFmtDesc = &av_pix_fmt_descriptors[pix_fmt];
#else
		while( ( pixFmtDesc = av_pix_fmt_desc_next( pixFmtDesc ) ) != NULL )
#endif
		{
			if( ! pixFmtDesc->name )
				continue;
			pixelFormats.push_back( std::string( pixFmtDesc->name ) );
		}
	}
	// specific video codec
	else
	{
		const AVCodec* videoCodec = avcodec_find_encoder_by_name( videoCodecName.c_str() );
		if( videoCodec && videoCodec->pix_fmts != NULL )
		{
			size_t pix_fmt = 0;
			while( videoCodec->pix_fmts[pix_fmt] != -1 )
			{
#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT( 51, 44, 0 )
				const AVPixFmtDescriptor* pix_desc = &av_pix_fmt_descriptors[ videoCodec->pix_fmts[pix_fmt] ];
#else
				const AVPixFmtDescriptor* pix_desc = av_pix_fmt_desc_get( videoCodec->pix_fmts[pix_fmt] );
#endif
				if( ! pix_desc || ! pix_desc->name )
					continue;
				pixelFormats.push_back( std::string( pix_desc->name ) );
				++pix_fmt;
			}
		}
	}
	return pixelFormats;
}

std::vector<std::string> getSampleFormats( const std::string& audioCodecName )
{
	std::vector<std::string> sampleFormats;
	
	// all audio codec concerned
	if( audioCodecName.empty() )
	{
		for( size_t sampleFormat = 0; sampleFormat < AV_SAMPLE_FMT_NB; ++sampleFormat)
		{
			sampleFormats.push_back( av_get_sample_fmt_name( static_cast<AVSampleFormat>( sampleFormat ) ) );
		}
	}
	// specific audio codec
	else
	{
		const AVCodec* audioCodec = avcodec_find_encoder_by_name( audioCodecName.c_str() );
		if( audioCodec && audioCodec->sample_fmts != NULL )
		{
			size_t sample_fmt = 0;
			while( audioCodec->sample_fmts[sample_fmt] != -1 )
			{
				const char* sampleFormatName = av_get_sample_fmt_name( audioCodec->sample_fmts[sample_fmt] );
				if( sampleFormatName )
					sampleFormats.push_back( std::string( sampleFormatName ) );
				sample_fmt++;
			}
		}
	}
	
	return sampleFormats;
}

AVPixelFormat getAVPixelFormat( const std::string& pixelFormat )
{
	return av_get_pix_fmt( pixelFormat.c_str() );
}

AVSampleFormat getAVSampleFormat( const std::string& sampleFormat )
{
	return av_get_sample_fmt( sampleFormat.c_str() );
}

std::vector<std::string> getFormatsLongNames()
{
	std::vector<std::string> formatsLongNames;

	AVOutputFormat* fmt = NULL;
	while( ( fmt = av_oformat_next( fmt ) ) )
	{
		// skip undefined codec
		if( fmt->video_codec == AV_CODEC_ID_NONE )
			continue;

		if( ! fmt->long_name )
			continue;

		formatsLongNames.push_back( std::string( fmt->long_name ) );
	}
	return formatsLongNames;
}
std::vector<std::string> getFormatsShortNames() 
{
	std::vector<std::string> formatsShortNames;

	AVOutputFormat* fmt = NULL;
	while( ( fmt = av_oformat_next( fmt ) ) )
	{
		// skip undefined codec
		if( fmt->video_codec == AV_CODEC_ID_NONE )
			continue;

		if( ! fmt->name )
			continue;

		formatsShortNames.push_back( std::string( fmt->name ) );
	}
	return formatsShortNames;
}

std::vector<std::string> getVideoCodecsLongNames()
{
	std::vector<std::string> videoCodecsLongNames;

	AVCodec* c = NULL;
	while( ( c = av_codec_next( c ) ) != NULL )
	{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 34, 0 )
		if( ! c->encode )
			continue;
#else
		if( ! c->encode2 )
			continue;
#endif
		switch( c->type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				if( ! c->long_name )
					continue;

				videoCodecsLongNames.push_back( std::string( c->long_name ) );
				break;
			}
			default:
				break;
		}
	}
	return videoCodecsLongNames;
}
std::vector<std::string> getVideoCodecsShortNames()
{
	std::vector<std::string> videoCodecsShortNames;

	AVCodec* c = NULL;
	while( ( c = av_codec_next( c ) ) != NULL )
	{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 34, 0 )
		if( ! c->encode )
			continue;
#else
		if( ! c->encode2 )
			continue;
#endif
		switch( c->type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				if( ! c->name )
					continue;

				videoCodecsShortNames.push_back( std::string( c->name ) );
				break;
			}
			default:
				break;
		}
	}
	return videoCodecsShortNames;
}

std::vector<std::string> getAudioCodecsLongNames()
{
	std::vector<std::string> audioCodecsLongNames;

	AVCodec* c = NULL;
	while( ( c = av_codec_next( c ) ) != NULL )
	{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 34, 0 )
		if( ! c->encode )
			continue;
#else
		if( ! c->encode2 )
			continue;
#endif
		switch( c->type )
		{
			case AVMEDIA_TYPE_AUDIO:
			{
				if( ! c->long_name )
					continue;

				audioCodecsLongNames.push_back( std::string( c->long_name ) );
				break;
			}
			default:
				break;
		}
	}
	return audioCodecsLongNames;
}
std::vector<std::string> getAudioCodecsShortNames()
{
	std::vector<std::string> audioCodecsShortNames;

	AVCodec* c = NULL;
	while( ( c = av_codec_next( c ) ) != NULL )
	{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 34, 0 )
		if( ! c->encode )
			continue;
#else
		if( ! c->encode2 )
			continue;
#endif
		switch( c->type )
		{
			case AVMEDIA_TYPE_AUDIO:
			{
				if( ! c->name )
					continue;

				audioCodecsShortNames.push_back( std::string( c->name ) );
				break;
			}
			default:
				break;
		}
	}
	return audioCodecsShortNames;
}

OptionArrayMap getOutputFormatOptions()
{
	av_register_all();

	std::map< std::string, std::vector<Option> > optionsPerFormat;
	
	AVOutputFormat* outputFormat = av_oformat_next( NULL );
	
	// iterate on formats
	while( outputFormat )
	{
		// add only format with video track
		// outputFormat->audio_codec ?
		if( outputFormat->video_codec != AV_CODEC_ID_NONE )
		{
			if( outputFormat->priv_class )
			{
				std::string outputFormatName( outputFormat->name );
				OptionArray options;
				loadOptions( options, (void*)&outputFormat->priv_class, 0  );
				optionsPerFormat.insert( std::make_pair( outputFormatName, options ) );
			}
		}
		outputFormat = av_oformat_next( outputFormat );
	}
	return optionsPerFormat;
}

OptionArrayMap getVideoCodecOptions()
{
	std::map< std::string, std::vector<Option> > videoCodecOptions;
	
	AVCodec* codec = av_codec_next( NULL );
	
	// iterate on codecs
	while( codec )
	{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 34, 0 )
		if( codec->encode )
#else
		if( codec->encode2 )
#endif
		{
			// add only video codec
			if( codec->type == AVMEDIA_TYPE_VIDEO )
			{
				if( codec->priv_class )
				{
					std::string videoCodecName( codec->name );
					OptionArray options;
					loadOptions( options, (void*)&codec->priv_class, 0  );
					videoCodecOptions.insert( std::make_pair( videoCodecName, options ) );
				}
			}
		}
		codec = av_codec_next( codec );
	}
	return videoCodecOptions;
}

OptionArrayMap getAudioCodecOptions()
{
	std::map< std::string, std::vector<Option> > audioCodecOptions;
	
	AVCodec* codec = av_codec_next( NULL );
	
	// iterate on codecs
	while( codec )
	{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 34, 0 )
		if( codec->encode )
#else
		if( codec->encode2 )
#endif
		{
			// add only audio codec
			if( codec->type == AVMEDIA_TYPE_AUDIO )
			{
				if( codec->priv_class )
				{
					std::string audioCodecName( codec->name );
					OptionArray options;
					loadOptions( options, (void*)&codec->priv_class, 0  );
					audioCodecOptions.insert( std::make_pair( audioCodecName, options ) );
				}
			}
		}
		codec = av_codec_next( codec );
	}
	return audioCodecOptions;
}

}
