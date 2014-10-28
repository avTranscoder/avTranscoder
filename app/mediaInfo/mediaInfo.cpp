#include <iostream>
#include <iomanip>
#include <sstream>

#include <mediaCore/Library.hpp>

int main( int argc, char** argv )
{
	using namespace mediamanager;

	mediacore::Libraries libs( mediacore::getLibraries() );

	for( mediacore::Libraries::iterator library = libs.begin(); library != libs.end(); ++library )
	{
		std::cout << std::left;
		std::cout << std::setw( 15 ) << (*library).getName();
		std::cout << std::setw( 10 ) << (*library).getStringVersion();
		std::cout << std::setw( 30 ) << (*library).getLicense();
		std::cout << std::endl;
	}

	// std::cout << "avinfo" << std::endl;
	std::vector<std::string> inputExtension  = mediacore::getInputExtensions();
	std::vector<std::string> outputExtension = mediacore::getOutputExtensions();
	
	std::cout << "----- inputExtension -----" << std::endl;
	std::cout << "nb inputExtension: " << inputExtension.size() << std::endl;
	for( std::vector<std::string>::iterator it = inputExtension.begin(); it != inputExtension.end(); ++it )
		std::cout << *it << std::endl;
	
	std::cout << "----- outputExtension -----" << std::endl;
	std::cout << "nb outputExtension: " << outputExtension.size() << std::endl;
	for( std::vector<std::string>::iterator it = outputExtension.begin(); it != outputExtension.end(); ++it )
		std::cout << *it << std::endl;
	
	return 0;
}
