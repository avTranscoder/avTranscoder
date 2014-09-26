#include <AvTranscoder/util.hpp>
#include <AvTranscoder/option/Context.hpp>
#include <AvTranscoder/option/Option.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavformat/avformat.h>
}

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <utility>

void displayOptions( std::vector<avtranscoder::Option>& options )
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

void optionChecker()
{
	// format options
	AVFormatContext* avFormatContext = avformat_alloc_context();
	avtranscoder::Context formatContext( avFormatContext );
	std::vector<avtranscoder::Option> formatOptions = formatContext.getOptions();
	displayOptions( formatOptions );

	// pixel formats
//	std::vector<std::string> pixelFormats = avtranscoder::getPixelFormats();
//	for( size_t i = 0; i < pixelFormats.size(); ++i )
//	{
//		std::cout << "----- " << pixelFormats[i] << " -----" << std::endl;
//	}

	// options per format
//	std::map< std::string, std::vector<avtranscoder::Option> > optionsPerFormat = avtranscoder::getOutputFormatOptions();
//	for( std::map< std::string, std::vector<avtranscoder::Option> >::iterator it = optionsPerFormat.begin();
//		it != optionsPerFormat.end();
//		++it )
//	{
//		std::cout << "----- " << it->first << " -----" << std::endl;
//		displayOptions( it->second );
//	}
}

int main( int argc, char** argv )
{
	std::cout << "start ..." << std::endl;

	try
	{
		optionChecker();
	}
	catch( std::exception &e )
	{
		std::cout << "[ERROR] " << e.what() << std::endl;
	}
	
	std::cout << "end ..." << std::endl;
}
