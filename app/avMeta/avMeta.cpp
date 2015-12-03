#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/mediaProperty/print.hpp>

#include <AvTranscoder/progress/NoDisplayProgress.hpp>

#include <iostream>

int main( int argc, char** argv )
{
	bool toJson = false;

	std::string help;
	help += "Usage\n";
	help += "\tavmeta INPUT_FILE [--json][--help]\n";
	help += "Command line options\n";
	help += "\t--json: print properties as json\n";

	if( argc < 2 )
	{
		std::cout << help << std::endl;
		return( 1 );
	}

	// List command line arguments
	std::vector< std::string > arguments;
	for( int argument = 1; argument < argc; ++argument )
	{
		arguments.push_back( argv[argument] );
	}
	for( size_t argument = 0; argument < arguments.size(); ++argument )
	{
		if( arguments.at( argument ) == "--help" )
		{
			std::cout << help << std::endl;
			return 0;
		}
		else if( arguments.at( argument ) == "--json" )
		{
			toJson = true;
		}
	}

	avtranscoder::preloadCodecsAndFormats();
	avtranscoder::Logger::setLogLevel( AV_LOG_QUIET );

	// analyse inputFile
	avtranscoder::InputFile input( argv[1] );
	avtranscoder::NoDisplayProgress p;
	input.analyse( p, avtranscoder::eAnalyseLevelFirstGop );

	// display file properties
	if( toJson )
		std::cout << input.getProperties().allPropertiesAsJson() << std::endl;
	else
		std::cout << input;
}
