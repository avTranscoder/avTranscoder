
#include <mediaCore/progress/NoDisplayProgress.hpp>

#include <mediaIO/file/InputFile.hpp>
#include <mediaIO/mediaProperty/printMediaProperty.hpp>

#include <iostream>

int main( int argc, char** argv )
{
	if( argc != 2 )
	{
		std::cout << "mediameta require a media filename" << std::endl;
		return( -1 );
	}

	using namespace mediaengine;

	mediacore::NoDisplayProgress p;

	mediaio::InputFile input( argv[1] );
	input.analyse( p, mediaio::InputFile::eAnalyseLevelFull );

	// a simply metadata display
	std::cout << input;
}
