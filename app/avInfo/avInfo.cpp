#include <iostream>
#include <iomanip>
#include <sstream>

#include <AvTranscoder/Library.hpp>

int main( int argc, char** argv )
{
	avtranscoder::Libraries libs( avtranscoder::getLibraries() );

	for( avtranscoder::Libraries::iterator library = libs.begin(); library != libs.end(); ++library )
	{
		std::cout << std::left;
		std::cout << std::setw( 15 ) << (*library).getName();
		std::cout << std::setw( 10 ) << (*library).getStringVersion();
		std::cout << std::setw( 30 ) << (*library).getLicense();
		std::cout << std::endl;
	}

	avtranscoder::preloadCodecsAndFormats();

	std::vector<std::string> inputExtension  = avtranscoder::getInputExtensions();
	std::vector<std::string> outputExtension = avtranscoder::getOutputExtensions();
	
	std::cout << "Supported input extensions: " << inputExtension.size() << std::endl;
	for( std::vector<std::string>::iterator it = inputExtension.begin(); it != inputExtension.end(); ++it )
		std::cout << *it << ", ";;
	
	std::cout << std::endl << std::endl << "Supported output extensions: " << outputExtension.size() << std::endl;
	for( std::vector<std::string>::iterator it = outputExtension.begin(); it != outputExtension.end(); ++it )
		std::cout << *it << ", ";
	std::cout << std::endl;
	return 0;
}
