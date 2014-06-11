#include <iostream>
#include <iomanip>

#include <AvTranscoder/Description.hpp>

int main( int argc, char** argv )
{
	// std::cout << "avinfo" << std::endl;
	std::vector<std::string> inputExtension  = avtranscoder::getInputExtensions();
	std::vector<std::string> outputExtension = avtranscoder::getOutputExtensions();
	return 0;
}
