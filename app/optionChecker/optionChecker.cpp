#include <AvTranscoder/util.hpp>
#include "AvTranscoder/option/FormatContext.hpp"
#include <AvTranscoder/option/CodecContext.hpp>
#include <AvTranscoder/option/Option.hpp>
#include <AvTranscoder/file/InputFile.hpp>

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <utility>

void displayOptions( const std::vector<avtranscoder::Option>& options )
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
			std::cout << "DefaultValue: " << option.getDefaultInt() << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeBool )
		{
			std::cout << "DefaultValue: " << option.getDefaultBool() << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeDouble )
		{
			std::cout << "DefaultValue: " << option.getDefaultDouble() << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeRatio )
		{
			std::cout << "DefaultValue: " << option.getDefaultRatio().first << ", " << option.getDefaultRatio().second << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeString )
		{
			std::cout << "DefaultValue: " << option.getDefaultString() << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeChoice )
		{
			std::cout << "Nb choices: " << option.getChilds().size() << std::endl;
			std::cout << "Default choice index: " << option.getDefaultChildIndex() << std::endl;
			for(size_t i = 0; i < option.getChilds().size(); ++i )
				std::cout << "Choice " << i << ": " << 
					option.getChildAtIndex( i ).getName() << " // " << 
					option.getChildAtIndex( i ).getHelp() << std::endl;
		}
		else if( option.getType() == avtranscoder::eOptionBaseTypeGroup )
		{
			std::cout << "Nb choices: " << option.getChilds().size() << std::endl;
			for(size_t i = 0; i < option.getChilds().size(); ++i )
				std::cout << "Element " << i << ": " << 
					option.getChildAtIndex( i ).getName() << " // " <<
					option.getChildAtIndex( i ).getDefaultBool() << std::endl;
		}
	}
}

void optionChecker( const std::string& inputfilename )
{
	avtranscoder::InputFile file( inputfilename );

	// format options
	avtranscoder::FormatContext formatContext;
	std::vector<avtranscoder::Option> formatOptions = formatContext.getOptions();
	displayOptions( formatOptions );

	// codec options
	AVCodecContext* avCodecContext = avcodec_alloc_context3( NULL );
	avtranscoder::CodecContext codecContext( *avCodecContext, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM );
	std::vector<avtranscoder::Option> codecOptions = codecContext.getOptions();
	displayOptions( codecOptions );

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

	if( argc <= 1 )
	{
		std::cout << "audiorewrapper require a media filename" << std::endl;
		std::cout << "example: audioWrap file.ext" << std::endl;
		return( -1 );
	}

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
