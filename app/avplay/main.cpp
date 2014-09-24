
#include <iostream>

#include "AvReader.hpp"
#include "Window.hpp"


int main( int argc, char** argv )
{
	AvReader avReader( argv[1] );
	Window window( avReader );
	window.launch();
}