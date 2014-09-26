#include "Context.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/opt.h>
}

#ifndef AV_OPT_FLAG_FILTERING_PARAM
 #define AV_OPT_FLAG_FILTERING_PARAM (1<<16)
#endif

#include <iostream>

namespace avtranscoder
{

std::vector<Option> Context::getOptions()
{
	std::vector<Option> optionsArray;
	for( std::map<std::string, Option>::iterator it = _options.begin(); it != _options.end(); ++it )
	{
		optionsArray.push_back( it->second );
	}
	return optionsArray;
}

void Context::loadOptions( void* av_class, int req_flags )
{	
	std::multimap<std::string, std::string> optionUnitToParentName;
	std::vector<Option> childOptions;
	
	const AVOption* avOption = NULL;
	
	// iterate on options
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 51, 12, 0 )
	while( ( avOption = av_next_option( av_class, avOption ) ) )
#else
	while( ( avOption = av_opt_next( av_class, avOption ) ) )
#endif
	{
		if( ! avOption || 
			! avOption->name ||
			( avOption->flags & req_flags ) != req_flags )
		{
			continue;
		}

		Option option( const_cast<AVOption*>( avOption ), av_class );

		if( option.getType() == eOptionBaseTypeChild )
		{
			childOptions.push_back( option );
		}
		else
		{
			_options.insert( std::make_pair( option.getName(), option ) );
			optionUnitToParentName.insert( std::make_pair( option.getUnit(), option.getName() ) );
		}
	}

	// iterate on child options
	for( std::vector<Option>::iterator itOption = childOptions.begin(); itOption != childOptions.end(); ++itOption )
	{
		bool parentFound = false;
		for( std::multimap<std::string, std::string>::iterator itUnit = optionUnitToParentName.begin(); itUnit != optionUnitToParentName.end(); ++itUnit )
		{
			if( itUnit->first == itOption->getUnit() )
			{
				std::string nameParentOption = itUnit->second;
				Option& parentOption = _options.at( nameParentOption );

				parentOption.appendChild( *itOption );

				// child of a Choice
				if( parentOption.getType() == eOptionBaseTypeChoice )
				{
					if( itOption->getDefaultValueInt() == parentOption.getDefaultValueInt() )
						parentOption.setDefaultChildIndex( parentOption.getNbChilds() - 1 );
				}

				parentFound = true;
				break;
			}
        }

		if( ! parentFound )
		{
			std::cout << "Warning: Can't find a choice option for " << itOption->getName() << std::endl;
		}
	}
}

}
