#include <iostream>

#include <AvTranscoder/Description.hpp>

int main( int argc, char** argv )
{
	// std::cout << "avinfo" << std::endl;
	std::vector<std::string> inputExtension  = avtranscoder::getInputExtensions();
	std::vector<std::string> outputExtension = avtranscoder::getOutputExtensions();
	
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
