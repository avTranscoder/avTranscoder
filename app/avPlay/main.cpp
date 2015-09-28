#include <AvTranscoder/common.hpp>
#include <AvTranscoder/reader/VideoReader.hpp>

#include "Window.hpp"


int main( int argc, char** argv )
{
	avtranscoder::preloadCodecsAndFormats();

	if(argc < 2)
	{
		std::cout << "avplay can play the given video media file." << std::endl;
		std::cout << "Provide the filename and the streamIndex (0 by default)" << std::endl; 
		return( -1 );
	}

	const std::string filename(argv[1]);
	const size_t streamIndex = argc > 2 ? atoi(argv[2]) : 0;
	
	avtranscoder::VideoReader reader( filename, streamIndex );
	Window window( reader );
	window.launch();
}
