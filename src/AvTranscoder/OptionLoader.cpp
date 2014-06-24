#include "OptionLoader.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/opt.h>
}

#ifndef AV_OPT_FLAG_FILTERING_PARAM
 #define AV_OPT_FLAG_FILTERING_PARAM (1<<16)
#endif

#include <string>
#include <map>
#include <utility> //pair
#include <exception>

namespace avtranscoder
{

OptionLoader::OptionLoader()
	: _options()
	, _avFormatContext( NULL )
	, _avCodecContext( NULL )
{
	_avFormatContext = avformat_alloc_context();
	
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 8, 0 )
	_avCodecContext = avcodec_alloc_context();
	// deprecated in the same version
	// _avCodecContext = avcodec_alloc_context2( AVMEDIA_TYPE_UNKNOWN );
#else
	AVCodec* avCodec = NULL;
	_avCodecContext = avcodec_alloc_context3( avCodec );
#endif
}

OptionLoader::~OptionLoader()
{
	avformat_free_context( _avFormatContext );
	avcodec_close( _avCodecContext );
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
		av_class = (void*)_avCodecContext;
	}
	else
	{
		av_class = (void*)_avFormatContext;
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
			_options.push_back( Option( *avOption, optionType ) );
			optionUnitToIndex.insert( 
				std::pair<std::string, int>( 
					std::string( avOption->unit ? avOption->unit : "" ), 
					_options.size() - 1 ) 
				);
		}
	}

	// iterate on childs option
	for( std::vector<Option>::iterator it = childOptions.begin(); it != childOptions.end(); ++it )
	{
		int indexParentOption = optionUnitToIndex.at( it->getUnit() );
		
		_options.at( indexParentOption ).appendChild( *it );
		
		// child of a Choice
		if( _options.at( indexParentOption ).getType() == TypeChoice )
		{
			if( it->getDefaultValueInt() == _options.at( indexParentOption ).getDefaultValueInt() )
				_options.at( indexParentOption ).setDefaultChildIndex( _options.at( indexParentOption ).getNbChilds() - 1 );
		}
	}
}

}
