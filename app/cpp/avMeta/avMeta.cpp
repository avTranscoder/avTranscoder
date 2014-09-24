#include <AvTranscoder/File/InputFile.hpp>
#include <AvTranscoder/Metadatas/Print.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>

#include <iostream>

int main( int argc, char** argv )
{
	if( argc != 2 )
	{
		std::cout << "avmeta require a media filename" << std::endl;
		return( -1 );
	}

	avtranscoder::NoDisplayProgress p;

	avtranscoder::InputFile input( argv[1] );
	input.analyse( p, avtranscoder::InputFile::eAnalyseLevelFull );

	// a simply metadata display
	std::cout << input;
}
