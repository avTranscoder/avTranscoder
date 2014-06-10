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
{
	
}

void OptionLoader::loadOptions( void* av_class, int req_flags, int rej_flags )
{
	std::map<std::string, int> optionUnitToIndex;
	std::vector<Option> childOptions;
	
	const AVOption* avOption = NULL;

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
		
		// child to a Choice
		if( m_options.at( indexParentOption ).getType() == TypeChoice )
		{
			if( it->getDefaultValueInt() == m_options.at( indexParentOption ).getDefaultValueInt() )
				m_options.at( indexParentOption ).setDefaultChildIndex( m_options.at( indexParentOption ).getNbChilds() - 1 );
		}
	}
}

}
