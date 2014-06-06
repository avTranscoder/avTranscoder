#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/DatasStructures/AudioDesc.hpp>

#include <AvTranscoder/Options/OptionInt.hpp>
#include <AvTranscoder/Options/OptionDouble.hpp>
#include <AvTranscoder/Options/OptionString.hpp>
#include <AvTranscoder/Option.hpp>

#include <vector>
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
			std::cout << "The option is " << avOption->name;
			std::cout << " of type : " << avOption->type << std::endl;
			
			avtranscoder::Option* opt = NULL;
			
			switch( avOption->type )
			{
				case AV_OPT_TYPE_INT:
				case AV_OPT_TYPE_INT64:
				{
					opt = new avtranscoder::OptionInt( *avOption );
					break;
				}
				case AV_OPT_TYPE_DOUBLE:
				{
					opt = new avtranscoder::OptionDouble( *avOption );
					break;
				}
				case AV_OPT_TYPE_STRING:
				{
					opt = new avtranscoder::OptionString( *avOption );
					break;
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
		
		switch( option->getType() )
		{
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				std::cout << "DefaultValue: " << option->getDefaultValue( valueInt ) << std::endl;
				break;
			}
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