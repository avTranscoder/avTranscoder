#include <AvTranscoder/common.hpp>

#include "AvReader.hpp"
#include "Window.hpp"


int main( int argc, char** argv )
{
	avtranscoder::preloadCodecsAndFormats();

	AvReader avReader( argv[1] );
	Window window( avReader );
	window.launch();
}