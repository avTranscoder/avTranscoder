#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/DatasStructures/AudioDesc.hpp>

#include <AvTranscoder/Options/OptionGroup.hpp>
#include <AvTranscoder/Options/OptionChoice.hpp>
#include <AvTranscoder/OptionLoader.hpp>

#include <string>
#include <iostream>
#include <iomanip>
#include <utility> //pair

int optionChecker( const std::string& inputfilename )
{
	avtranscoder::InputFile inputFile( inputfilename );
	inputFile.analyse();
	
	avtranscoder::AudioDesc audioDesc( inputFile.getStream( 0 ).getAudioDesc() );
	
	avtranscoder::OptionLoader optionLoader;
	optionLoader.loadOptions( audioDesc.getCodecContext() );
	
	// display Options
	for( auto option : optionLoader.getOptions() )
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
		std::pair<int, int> value2D;
		
		if( option->getType() == "OptionInt" )
		{
			std::cout << "DefaultValue: " << option->getDefaultValue( valueInt ) << std::endl;
		}
		else if( option->getType() == "OptionBoolean" )
		{
			std::cout << "DefaultValue: " << option->getDefaultValue( valueBool ) << std::endl;
		}
		else if( option->getType() == "OptionDouble" )
		{
			std::cout << "DefaultValue: " << option->getDefaultValue( valueDouble ) << std::endl;
		}
		else if( option->getType() == "OptionRatio" )
		{
			option->getDefaultValue( value2D );
			std::cout << "DefaultValue: " << value2D.first << ", " << value2D.second << std::endl;
		}
		else if( option->getType() == "OptionString" )
		{
			std::cout << "DefaultValue: " << option->getDefaultValue( valueStr ) << std::endl;
		}
		else if( option->getType() == "OptionChoice" )
		{
			std::cout << "DefaultValue: " << option->getDefaultValue( valueDouble ) << std::endl;
			
			avtranscoder::OptionChoice* choice = dynamic_cast<avtranscoder::OptionChoice*>( option );
			std::cout << "Nb choices: " << choice->getNbChoices() << std::endl;
			std::cout << "Default choice index: " << choice->getDefaultChoiceIndex() << std::endl;
			for(size_t i = 0; i < choice->getNbChoices(); ++i )
				std::cout << "Choice " << i << ": " << choice->getChoice( i ).first << " // " << choice->getChoice( i ).second << std::endl;
		}
		else if( option->getType() == "OptionGroup" )
		{
			std::cout << "DefaultValue: " << option->getDefaultValue( valueInt ) << std::endl;
			
			avtranscoder::OptionGroup* group = dynamic_cast<avtranscoder::OptionGroup*>( option );
			std::cout << "Nb choices: " << group->getNbElements() << std::endl;
			for(size_t i = 0; i < group->getNbElements(); ++i )
				std::cout << "Element " << i << ": " << 
					group->getElement( i ).getName() << " // " <<
					group->getElement( i ).getDefaultValue( valueBool ) << std::endl;
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