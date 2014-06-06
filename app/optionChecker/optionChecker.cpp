#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/DatasStructures/AudioDesc.hpp>

#include <AvTranscoder/Options/OptionInt.hpp>
#include <AvTranscoder/Options/OptionDouble.hpp>
#include <AvTranscoder/Options/OptionString.hpp>
#include <AvTranscoder/Options/OptionBoolean.hpp>
#include <AvTranscoder/Options/Option2D.hpp>
#include <AvTranscoder/Options/OptionGroup.hpp>
#include <AvTranscoder/Option.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

int optionChecker( const std::string& inputfilename )
{
	avtranscoder::InputFile inputFile( inputfilename );
	inputFile.analyse();
	
	avtranscoder::AudioDesc audioDesc( inputFile.getStream( 0 ).getAudioDesc() );
	
	std::vector<avtranscoder::Option*> options;
	try
	{
		const AVOption* avOption = NULL;
		
		while( ( avOption = av_opt_next( audioDesc.getCodecContext(), avOption ) ) != NULL )
		{
			avtranscoder::Option* opt = NULL;
			
			if( !avOption || ! avOption->name )
			{
				continue;
			}
			
			std::cout << "The option is " << avOption->name << " of type : " << avOption->type << std::endl;

			if( avOption->unit && avOption->type == AV_OPT_TYPE_FLAGS )
			{
				opt = new avtranscoder::OptionGroup( *avOption );
				options.push_back( opt );
				continue;
			}
			if( avOption->unit && avOption->type == AV_OPT_TYPE_INT )
			{
				// OptionChoice
				//options.push_back( opt );
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
					opt = new avtranscoder::Option2D( *avOption );
					break;
				}
				case AV_OPT_TYPE_BINARY:
				{
					//opt = new avtranscoder::OptionString( *avOption );
					break;
				}
				case AV_OPT_TYPE_CONST:
				{
					break;
				}
				default:
				{
					//throw std::runtime_error( "undefined type for " + std::string( avOption->name ) );
					std::cout << "----- Unknowed type for " << avOption->name << "-----" << std::endl;
				}
			}
			if( opt )
				options.push_back( opt );
		}
	}
	catch( std::exception &e )
	{
		for( auto it = options.begin(); it != options.end(); ++it)
		{
			delete *it;
		} 
		options.clear();
		
		throw e;
	}
	
	for( auto option : options )
	{
		std::cout << std::left;
		std::cout << "****************************" << std::endl;
		std::cout << std::setw(30) << option->getName();
		std::cout << ": " << option->getHelp() << std::endl;
		std::cout << "Type: " << option->getType() << std::endl;
		
		// get default value
		int valueInt;
		double valueDouble;
		std::string valueStr;
		bool valueBool;
		avtranscoder::Value2D value2D;
		
		switch( option->getType() )
		{
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				std::cout << "DefaultValue: " << option->getDefaultValue( valueInt ) << std::endl;
				break;
			}
			case AV_OPT_TYPE_FLOAT:
			case AV_OPT_TYPE_DOUBLE:
			{
				std::cout << "DefaultValue: " << option->getDefaultValue( valueDouble ) << std::endl;
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				std::cout << "DefaultValue: " << option->getDefaultValue( valueStr ) << std::endl;
				break;
			}
			case AV_OPT_TYPE_FLAGS:
			{
				if( option->getName().substr(0, 2) == "g_" ) // OptionGroup
					std::cout << "DefaultValue: " << option->getDefaultValue( valueInt ) << std::endl;
				else // OptionBoolean
					std::cout << "DefaultValue: " << option->getDefaultValue( valueBool ) << std::endl;
				break;
			}
			case AV_OPT_TYPE_RATIONAL:
			{
				option->getDefaultValue( value2D );
				std::cout << "DefaultValue: " << value2D.num << ", " << value2D.dem << std::endl;
				break;
			}
		}
	}

}

int main( int argc, char** argv )
{
	if( argc <= 1 )
	{
		std::cout << "audiorewrapper require a media filename" << std::endl;
		std::cout << "example: audioWrap file.ext" << std::endl;
		return( -1 );
	}

	std::cout << "start ..." << std::endl;

	try
	{
		optionChecker( argv[1] );
	}
	catch( std::exception &e )
	{
		std::cout << "[ERROR] " << e.what() << std::endl;
	}
	
	std::cout << "end ..." << std::endl;
}