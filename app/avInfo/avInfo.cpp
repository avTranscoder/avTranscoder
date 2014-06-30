#include <iostream>
#include <iomanip>
#include <sstream>

#include <AvTranscoder/Description.hpp>

int main( int argc, char** argv )
{
	avtranscoder::AvVersions versions( avtranscoder::getVersion() );

	for( avtranscoder::AvVersions::iterator libVersion = versions.begin(); libVersion != versions.end(); ++libVersion )
	{
		std::cout << std::left << std::setw( 15 ) << (*libVersion).first;
		std::cout << std::left << std::setw( 30 ) << avtranscoder::getLicence();

		std::stringstream completeVersion;
		for( std::vector<size_t>::iterator version = (*libVersion).second.begin(); version != (*libVersion).second.end(); ++version )
		{
			completeVersion << *version << ".";
		}
		std::cout << completeVersion.str() << std::endl;
	}

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
