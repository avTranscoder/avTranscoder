#include <AvTranscoder/option/OptionLoader.hpp>
#include <AvTranscoder/option/Option.hpp>

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <utility> //pair

void displayOptions( avtranscoder::OptionLoader::OptionArray& options )
{
	for( auto option : options )
	{
		std::cout << std::left;
		std::cout << "****************************" << std::endl;
		std::cout << "Name: " << option.getName() << std::endl;
		std::cout << "Unit : " << option.getUnit() << std::endl;
		std::cout << "Help: " << option.getHelp() << std::endl;
		std::cout << "Type: " << option.getType() << std::endl;
		
		// get default value
		
		if( option.getType() == avtranscoder::eOptionBaseTypeInt )
		{
			std::cout << "DefaultValue: " << option.getDefaultValueInt() << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeBool )
		{
			std::cout << "DefaultValue: " << option.getDefaultValueBool() << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeDouble )
		{
			std::cout << "DefaultValue: " << option.getDefaultValueDouble() << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeRatio )
		{
			std::cout << "DefaultValue: " << option.getDefaultValueRatio().first << ", " << option.getDefaultValueRatio().second << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeString )
		{
			std::cout << "DefaultValue: " << option.getDefaultValueString() << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeChoice )
		{
			std::cout << "Nb choices: " << option.getNbChilds() << std::endl;
			std::cout << "Default choice index: " << option.getDefaultChildIndex() << std::endl;
			for(size_t i = 0; i < option.getNbChilds(); ++i )
				std::cout << "Choice " << i << ": " << 
					option.getChild( i ).getName() << " // " << 
					option.getChild( i ).getHelp() << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeGroup )
		{
			std::cout << "Nb choices: " << option.getNbChilds() << std::endl;
			for(size_t i = 0; i < option.getNbChilds(); ++i )
				std::cout << "Element " << i << ": " << 
					option.getChild( i ).getName() << " // " <<
					option.getChild( i ).getDefaultValueBool() << std::endl;
		}
	}
}

void optionChecker( const std::string& inputfilename )
{	
	avtranscoder::OptionLoader optionLoader;
	
	//avtranscoder::OptionLoader::OptionArray optionsArray = optionLoader.loadOptions( AV_OPT_FLAG_AUDIO_PARAM );
	avtranscoder::OptionLoader::OptionMap optionsMap = optionLoader.loadOutputFormatOptions();
	
	//displayOptions( optionsArray );
	for( avtranscoder::OptionLoader::OptionMap::iterator it = optionsMap.begin();
		it != optionsMap.end();
		++it )
	{
		std::cout << "----- " << it->first << " -----" << std::endl;
		displayOptions( it->second );
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
