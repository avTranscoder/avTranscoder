#include "OptionLoader.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/opt.h>
#include <libavutil/pixfmt.h>
#include <libavutil/pixdesc.h>
}

#ifndef AV_OPT_FLAG_FILTERING_PARAM
 #define AV_OPT_FLAG_FILTERING_PARAM (1<<16)
#endif

#include <string>
#include <map>
#include <utility> //pair
#include <iostream>

namespace avtranscoder
{

OptionLoader::OptionLoader()
	: _avFormatContext( NULL )
	, _avCodecContext( NULL )
	, _outputFormat( NULL )
	, _codec( NULL )
	, _formatsLongNames()
	, _formatsShortNames()
	, _videoCodecsLongNames()
	, _videoCodecsShortNames()
	, _audioCodecsLongNames()
	, _audioCodecsShortNames()
{
	// Alloc format context
	_avFormatContext = avformat_alloc_context();
	
	// Alloc codec context
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 8, 0 )
	_avCodecContext = avcodec_alloc_context();
	// deprecated in the same version
	// _avCodecContext = avcodec_alloc_context2( AVMEDIA_TYPE_UNKNOWN );
#else
	AVCodec* avCodec = NULL;
	_avCodecContext = avcodec_alloc_context3( avCodec );
#endif
	
	// fill format short and long names
	AVOutputFormat* fmt = NULL;
	while( ( fmt = av_oformat_next( fmt ) ) )
	{
		// add only format with video track
		if( fmt->video_codec == AV_CODEC_ID_NONE )
			continue;
		
		if( ! fmt->long_name )
			continue;
		
		_formatsLongNames.push_back( std::string( fmt->long_name ) );
		_formatsShortNames.push_back( std::string( fmt->name ) );
	}
	
	// fill video and audio codec short and long names
	AVCodec* c = NULL;
	while( ( c = av_codec_next( c ) ) != NULL )
	{
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT( 53, 34, 0 )
		if( ! c->encode2 )
			continue;
#else
		if( ! c->encode )
			continue;
#endif
		switch( c->type )
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				if( ! c->long_name )
					continue;

				_videoCodecsLongNames.push_back( std::string( c->long_name ) );
				_videoCodecsShortNames.push_back( std::string( c->name ) );
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				if( ! c->long_name )
					continue;

				_audioCodecsLongNames.push_back( std::string( c->long_name ) );
				_audioCodecsShortNames.push_back( std::string( c->name ) );
				break;
			}
			default:
				break;
		}
	}
}

OptionLoader::~OptionLoader()
{
	avformat_free_context( _avFormatContext );
	avcodec_close( _avCodecContext );
}

OptionLoader::OptionArray OptionLoader::loadFormatContextOptions( int req_flags )
{
	return loadOptions( (void*)_avFormatContext, req_flags );
}

OptionLoader::OptionArray OptionLoader::loadCodecContextOptions( int req_flags )
{
	return loadOptions( (void*)_avCodecContext, req_flags );
}

OptionLoader::OptionMap OptionLoader::loadOutputFormatOptions()
{
	OptionMap outputFormatOptions;
	
	_outputFormat = av_oformat_next( NULL );
	
	// iterate on formats
	while( _outputFormat )
	{
		// add only format with video track
		// _outputFormat->audio_codec ?
		if( _outputFormat->video_codec != AV_CODEC_ID_NONE )
		{
			if( _outputFormat->priv_class )
			{
				std::string outputFormatName( _outputFormat->name );
				OptionArray optionsArray = loadOptions( (void*)&_outputFormat->priv_class );

				outputFormatOptions.insert( 
					std::pair< std::string, OptionArray >( 
						outputFormatName,
						optionsArray )
					);
			}
		}
		_outputFormat = av_oformat_next( _outputFormat );
	}
	return outputFormatOptions;
}

OptionLoader::OptionMap OptionLoader::loadVideoCodecOptions()
{
	OptionMap videoCodecOptions;
	
	_codec = av_codec_next( NULL );
	
	// iterate on codecs
	while( _codec )
	{
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT( 53, 34, 0 )
		if( _codec->encode2 )
#else
		if( _codec->encode )
#endif
		{
			// add only video codec
			if( _codec->type == AVMEDIA_TYPE_VIDEO )
			{
				if( _codec->priv_class )
				{
					std::string videoCodecName( _codec->name );
					OptionArray optionsArray = loadOptions( (void*)&_codec->priv_class );
					
					videoCodecOptions.insert( 
						std::pair< std::string, OptionArray >( 
							videoCodecName,
							optionsArray )
						);
				}
			}
		}
		_codec = av_codec_next( _codec );
	}
	return videoCodecOptions;
}

OptionLoader::OptionMap OptionLoader::loadAudioCodecOptions()
{
	OptionMap audioCodecOptions;
	
	_codec = av_codec_next( NULL );
	
	// iterate on codecs
	while( _codec )
	{
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT( 53, 34, 0 )
		if( _codec->encode2 )
#else
		if( _codec->encode )
#endif
		{
			// add only audio codec
			if( _codec->type == AVMEDIA_TYPE_AUDIO )
			{
				if( _codec->priv_class )
				{
					std::string audioCodecName( _codec->name );
					OptionArray optionsArray = loadOptions( (void*)&_codec->priv_class );
					
					audioCodecOptions.insert( 
						std::pair< std::string, OptionArray >( 
							audioCodecName,
							optionsArray )
						);
				}
			}
		}
		_codec = av_codec_next( _codec );
	}
	return audioCodecOptions;
}

OptionLoader::OptionArray OptionLoader::loadOptions( void* av_class, int req_flags )
{
	OptionArray options;
	
	std::map<std::string, int> optionUnitToIndex;
	std::vector<Option> childOptions;
	
	const AVOption* avOption = NULL;
	
	// iterate on options
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 51, 12, 0 )
	while( ( avOption = av_next_option( av_class, avOption ) ) )
#else
	while( ( avOption = av_opt_next( av_class, avOption ) ) )
#endif
	{
		if( !avOption || 
			! avOption->name ||
			( avOption->flags & req_flags ) != req_flags )
		{
			continue;
		}

		OptionType optionType = Option::getTypeFromAVOption( avOption->unit, avOption->type );

		//std::cout << "The option is " << avOption->name << " of type : " << avOption->type << std::endl;

		if( optionType == TypeChild )
		{
			childOptions.push_back( Option( *avOption, optionType ) );
		}
		else
		{
			options.push_back( Option( *avOption, optionType ) );
			optionUnitToIndex.insert( 
				std::pair<std::string, int>( 
					std::string( avOption->unit ? avOption->unit : "" ), 
					options.size() - 1 ) 
				);
		}
	}

	// iterate on child options
	for( std::vector<Option>::iterator it = childOptions.begin(); it != childOptions.end(); ++it )
	{
		int indexParentOption = optionUnitToIndex.at( it->getUnit() );
		Option& parentOption = options.at( indexParentOption );
		
		parentOption.appendChild( *it );
		
		// child of a Choice
		if( parentOption.getType() == TypeChoice )
		{
			if( it->getDefaultValueInt() == parentOption.getDefaultValueInt() )
				parentOption.setDefaultChildIndex( parentOption.getNbChilds() - 1 );
		}
	}
	return options;
}

std::vector<std::string> OptionLoader::getPixelFormats ( const std::string& videoCodecName ) const
{
	std::vector<std::string> pixelFormats;
	
	// all video codec concerned
	if( videoCodecName == "" )
	{
		const AVPixFmtDescriptor* pixFmtDesc = NULL; 
		while( ( pixFmtDesc = av_pix_fmt_desc_next( pixFmtDesc ) ) != NULL )
		{
			if( ! pixFmtDesc->name )
				continue;
			pixelFormats.push_back( std::string( pixFmtDesc->name ) );
		}
	}
	// specific video codec
	else
	{
		AVCodec* videoCodec = avcodec_find_encoder_by_name( videoCodecName.c_str() );

		if( videoCodec->pix_fmts != NULL )
		{
			size_t pix_fmt = 0;
			while( videoCodec->pix_fmts[pix_fmt] != -1 )
			{
				const AVPixFmtDescriptor* pix_desc = av_pix_fmt_desc_get( videoCodec->pix_fmts[pix_fmt] );
				if( ! pix_desc->name )
					continue;
				pixelFormats.push_back( std::string( pix_desc->name ) );
				++pix_fmt;
			}
		}
	}
	return pixelFormats;
}

}
