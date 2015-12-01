#include <AvTranscoder/common.hpp>
#include <AvTranscoder/reader/VideoReader.hpp>

#include "Window.hpp"

#include <cstdlib>


int main( int argc, char** argv )
{
	std::string filename;
	size_t streamIndex = 0;

	std::string help;
	help += "Usage\n";
	help += "\tavplay filename [streamIndex] [--help]\n";
	help += "Command line options\n";
	help += "\tstreamIndex: index of the stream to read (by default 0)\n";
	help += "\t--help: display this help\n";

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
		// positional arguments
		if( argument == 0 )
		{
			filename = arguments.at( argument );
		}
		else if( argument == 1 )
		{
			streamIndex = atoi( arguments.at( argument ).c_str() );
		}
	}

	// Check required arguments
	if(argc < 2)
	{
		std::cout << "avplay can play the given video media file." << std::endl;
		std::cout << "Use option --help to display help" << std::endl; 
		return( -1 );
	}

	// Setup avtranscoder
	avtranscoder::preloadCodecsAndFormats();
	
	avtranscoder::VideoReader reader( filename, streamIndex );
	Window window( reader );
	window.launch();
}
