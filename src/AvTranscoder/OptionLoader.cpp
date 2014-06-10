#include "OptionLoader.hpp"

#include <AvTranscoder/Options/OptionInt.hpp>
#include <AvTranscoder/Options/OptionDouble.hpp>
#include <AvTranscoder/Options/OptionString.hpp>
#include <AvTranscoder/Options/OptionBoolean.hpp>
#include <AvTranscoder/Options/OpionRatio.hpp>
#include <AvTranscoder/Options/OptionGroup.hpp>
#include <AvTranscoder/Options/OptionChoice.hpp>

#include <exception>
#include <string>

namespace avtranscoder
{

OptionLoader::OptionLoader()
	: m_options()
{
	
}

void OptionLoader::loadOptions( void* av_class )
{
	// tmp vectors to access easely to the OptionChoice/OptionGroup to add choice / boolean to them
	std::vector<avtranscoder::OptionChoice*> optionsChoice; 
	std::vector<avtranscoder::OptionGroup*> optionsGroup;
	try
	{
		const AVOption* avOption = NULL;
		
		while( ( avOption = av_opt_next( av_class, avOption ) ) != NULL )
		{	
			if( !avOption || ! avOption->name )
			{
				continue;
			}
			
			avtranscoder::Option* opt = NULL;
			
			//std::cout << "The option is " << avOption->name << " of type : " << avOption->type << std::endl;

			if( avOption->unit && avOption->type == AV_OPT_TYPE_FLAGS )
			{
				opt = new avtranscoder::OptionGroup( *avOption );
				m_options.push_back( opt );
				optionsGroup.push_back( dynamic_cast<avtranscoder::OptionGroup*>( opt ) );
				continue;
			}
			if( avOption->unit && avOption->type == AV_OPT_TYPE_INT )
			{
				opt = new avtranscoder::OptionChoice( *avOption );
				m_options.push_back( opt );
				optionsChoice.push_back( dynamic_cast<avtranscoder::OptionChoice*>( opt ) );
				continue;
			}

			switch( avOption->type )
			{
				case AV_OPT_TYPE_FLAGS:
				{
					opt = new avtranscoder::OptionBoolean( *avOption );
					break;
				}
				case AV_OPT_TYPE_INT:
				case AV_OPT_TYPE_INT64:
				{
					opt = new avtranscoder::OptionInt( *avOption );
					break;
				}
				case AV_OPT_TYPE_DOUBLE:
				case AV_OPT_TYPE_FLOAT:
				{
					opt = new avtranscoder::OptionDouble( *avOption );
					break;
				}
				case AV_OPT_TYPE_STRING:
				{
					opt = new avtranscoder::OptionString( *avOption );
					break;
				}
				case AV_OPT_TYPE_RATIONAL:
				{
					opt = new avtranscoder::OpionRatio( *avOption );
					break;
				}
				case AV_OPT_TYPE_BINARY:
				{
					opt = new avtranscoder::OptionString( *avOption );
					break;
				}
				case AV_OPT_TYPE_CONST:
				{
					break;
				}
				default:
				{
					//std::cout << "----- Unknowed type for " << avOption->name << "-----" << std::endl;
					break;
				}
			}
			if( opt )
				m_options.push_back( opt );
		}
		
		// adding enum values and set default value for enums
		avOption = NULL;
		while( (avOption = av_opt_next( av_class, avOption)  ) != NULL )
		{
			if( !avOption ||
				!avOption->name ||
				( avOption->unit && avOption->type == AV_OPT_TYPE_FLAGS ) ||
				( avOption->unit && avOption->type == AV_OPT_TYPE_INT ) )
			{
				continue;
			}
			
			avtranscoder::OptionBoolean* optBoolean = NULL;

			switch( avOption->type )
			{
				case AV_OPT_TYPE_CONST:
				{
					for( size_t i = 0; i < optionsChoice.size(); i++ )
					{
						if( avOption->unit == optionsChoice.at( i )->getUnit() )
						{
							if( avOption->help )
								optionsChoice.at( i )->appendChoice( avOption->name, avOption->help );
							else
								optionsChoice.at( i )->appendChoice( avOption->name );
							
							double valueDouble;
							if( avOption->default_val.dbl == optionsChoice.at( i )->getDefaultValue( valueDouble ) )
								optionsChoice.at( i )->setDefaultChoiceIndex( optionsChoice.at( i )->getNbChoices() - 1 );
						}
					}
					
					for( size_t i = 0; i < optionsGroup.size(); i++ )
					{
						std::string name = "g_";
						name += avOption->unit;
						if( name == optionsGroup.at( i )->getName() )
						{
							optBoolean = new avtranscoder::OptionBoolean( *avOption );
							optionsGroup.at( i )->appendElement( *optBoolean );
							break;
						}
					}
				}
				default:
				{
					break;
				}
			}
		}
	}
	
	catch( std::exception &e )
	{
		for( std::vector<Option*>::iterator it = m_options.begin(); it != m_options.end(); ++it)
		{
			delete *it;
		} 
		m_options.clear();
		
		throw e;
	}
}

}
