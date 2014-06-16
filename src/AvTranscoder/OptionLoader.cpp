#include "OptionLoader.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/opt.h>
}

#include <string>
#include <map>
#include <utility> //pair
#include <iostream>

namespace avtranscoder
{

OptionLoader::OptionLoader()
	: m_avFormatContext( NULL )
	, m_avCodecContext( NULL )
	, m_outputFormat( NULL )
	, m_codec( NULL )
{
	// Alloc format context
	m_avFormatContext = avformat_alloc_context();
	
	// Alloc codec context
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 8, 0 )
	m_avCodecContext = avcodec_alloc_context();
	// deprecated in the same version
	//m_avCodecContext = avcodec_alloc_context2( AVMEDIA_TYPE_UNKNOWN );
#else
	AVCodec* avCodec = NULL;
	m_avCodecContext = avcodec_alloc_context3( avCodec );
#endif
}

OptionLoader::~OptionLoader()
{
	avformat_free_context( m_avFormatContext );
	av_free( m_avCodecContext );
}

OptionLoader::OptionArray OptionLoader::loadFormatContextOptions( int req_flags )
{
	return loadOptions( (void*)m_avFormatContext, req_flags );
}

OptionLoader::OptionArray OptionLoader::loadCodecContextOptions( int req_flags )
{
	return loadOptions( (void*)m_avCodecContext, req_flags );
}

OptionLoader::OptionMap OptionLoader::loadOutputFormatOptions()
{
	OptionMap outputFormatOptions;
	
	m_outputFormat = av_oformat_next( NULL );
	
	// iterate on formats
	while( m_outputFormat )
	{
		// add only format with video track
		// m_outputFormat->audio_codec ?
		if( m_outputFormat->video_codec != AV_CODEC_ID_NONE )
		{
			if( m_outputFormat->priv_class )
			{
				std::string outputFormatName( m_outputFormat->name );
				OptionArray optionsArray = loadOptions( (void*)&m_outputFormat->priv_class );

				outputFormatOptions.insert( 
					std::pair< std::string, OptionArray >( 
						outputFormatName,
						optionsArray )
					);
			}
		}
		m_outputFormat = av_oformat_next( m_outputFormat );
	}
	return outputFormatOptions;
}

OptionLoader::OptionMap OptionLoader::loadVideoCodecOptions()
{
	OptionMap videoCodecOptions;
	
	m_codec = av_codec_next( NULL );
	
	// iterate on codecs
	while( m_codec )
	{
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT( 53, 34, 0 )
		if( m_codec->encode2 )
#else
		if( m_codec->encode )
#endif
		{
			// add only video codec
			if( m_codec->type == AVMEDIA_TYPE_VIDEO )
			{
				if( m_codec->priv_class )
				{
					std::string videoCodecName( m_codec->name );
					OptionArray optionsArray = loadOptions( (void*)&m_codec->priv_class );
					
					videoCodecOptions.insert( 
						std::pair< std::string, OptionArray >( 
							videoCodecName,
							optionsArray )
						);
				}
			}
		}
		m_codec = av_codec_next( m_codec );
	}
	return videoCodecOptions;
}

OptionLoader::OptionMap OptionLoader::loadAudioCodecOptions()
{
	OptionMap audioCodecOptions;
	
	m_codec = av_codec_next( NULL );
	
	// iterate on codecs
	while( m_codec )
	{
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT( 53, 34, 0 )
		if( m_codec->encode2 )
#else
		if( m_codec->encode )
#endif
		{
			// add only audio codec
			if( m_codec->type == AVMEDIA_TYPE_AUDIO )
			{
				if( m_codec->priv_class )
				{
					std::string audioCodecName( m_codec->name );
					OptionArray optionsArray = loadOptions( (void*)&m_codec->priv_class );
					
					audioCodecOptions.insert( 
						std::pair< std::string, OptionArray >( 
							audioCodecName,
							optionsArray )
						);
				}
			}
		}
		m_codec = av_codec_next( m_codec );
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

}
