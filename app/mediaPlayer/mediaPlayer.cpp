
#include <iostream>

#include "MediaReader.hpp"
#include "Window.hpp"


int main( int argc, char** argv )
{
	MediaReader mediaReader( argv[1] );
	Window window( mediaReader );
	window.launch();
}