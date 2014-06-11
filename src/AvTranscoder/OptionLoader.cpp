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
#include <exception>

namespace avtranscoder
{

OptionLoader::OptionLoader()
	: m_options()
	, m_avFormatContext( NULL )
	, m_avCodecContext( NULL )
{
	m_avFormatContext = avformat_alloc_context();
	
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
	avcodec_close( m_avCodecContext );
}

void OptionLoader::loadOptions( int req_flags, int rej_flags )
{
	std::map<std::string, int> optionUnitToIndex;
	std::vector<Option> childOptions;
	
	const AVOption* avOption = NULL;
	
	// get ffmpeg / libav object on which we'll scan AVOption
	void* av_class = NULL;
	if( ( req_flags & AV_OPT_FLAG_VIDEO_PARAM ) == AV_OPT_FLAG_VIDEO_PARAM ||
		( req_flags & AV_OPT_FLAG_AUDIO_PARAM ) == AV_OPT_FLAG_AUDIO_PARAM ||
		( req_flags & AV_OPT_FLAG_METADATA ) == AV_OPT_FLAG_METADATA ||
		( req_flags & AV_OPT_FLAG_FILTERING_PARAM ) == AV_OPT_FLAG_FILTERING_PARAM ||
		( req_flags & AV_OPT_FLAG_SUBTITLE_PARAM ) == AV_OPT_FLAG_SUBTITLE_PARAM )
	{
		av_class = (void*)m_avCodecContext;
	}
	else
	{
		av_class = (void*)m_avFormatContext;
	}
	
	// iterate on options
	while( ( avOption = av_opt_next( av_class, avOption ) ) != NULL )
	{	
		if( !avOption || 
			! avOption->name ||
			( avOption->flags & req_flags ) != req_flags ||
			( avOption->flags & rej_flags ) )
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
			m_options.push_back( Option( *avOption, optionType ) );
			optionUnitToIndex.insert( 
				std::pair<std::string, int>( 
					std::string( avOption->unit ? avOption->unit : "" ), 
					m_options.size() - 1 ) 
				);
		}
	}

	// iterate on childs option
	for( std::vector<Option>::iterator it = childOptions.begin(); it != childOptions.end(); ++it )
	{
		int indexParentOption = optionUnitToIndex.at( it->getUnit() );
		
		m_options.at( indexParentOption ).appendChild( *it );
		
		// child of a Choice
		if( m_options.at( indexParentOption ).getType() == TypeChoice )
		{
			if( it->getDefaultValueInt() == m_options.at( indexParentOption ).getDefaultValueInt() )
				m_options.at( indexParentOption ).setDefaultChildIndex( m_options.at( indexParentOption ).getNbChilds() - 1 );
		}
	}
}

}
