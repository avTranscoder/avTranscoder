
#include <mediaCore/Library.hpp>
#include <mediaCore/progress/NoDisplayProgress.hpp>

#include <mediaIO/file/InputFile.hpp>
#include <mediaIO/mediaProperty/printMediaProperty.hpp>

#include <iostream>

int main( int argc, char** argv )
{
	using namespace mediaengine;

	bool help = false;
	bool version = false;

	for( size_t argIndex = 1; argIndex < argc; ++argIndex )
	{
		std::string arg( argv[argIndex]);
		if( !strcmp( argv[argIndex], "-h") ||
			!strcmp( argv[argIndex], "-H") ||
			!strcmp( argv[argIndex], "--help") )
			help = true;

		if( !strcmp( argv[argIndex], "-V") ||
			!strcmp( argv[argIndex], "--version" ) )
			version = true;
	}

	if( help )
	{
		std::cout << "mediameta - " << MEDIA_ENGINE_VERSION_MAJOR << "." << MEDIA_ENGINE_VERSION_MINOR << "." << MEDIA_ENGINE_VERSION_MICRO << std::endl;
		std::cout << std::endl;
		std::cout << "display media metadatas for each input file" << std::endl;
		std::cout << std::endl;
		std::cout << "mediameta [OPTIONS] [FILENAMES]" << std::endl;
		std::cout << std::endl;
		std::cout << "OPTIONS:" << std::endl;
		std::cout << "-h, -H, --help: Display this help" << std::endl;
		std::cout << "-v, --version:  Print library version and FFMpeg/Libav version." << std::endl;
		std::cout << "                Also display license." << std::endl;
		return 0;
	}

	if( version )
	{
		mediacore::Libraries libs( mediacore::getLibraries() );

		for( mediacore::Libraries::iterator library = libs.begin(); library != libs.end(); ++library )
		{
			std::cout << std::left;
			std::cout << std::setw( 15 ) << (*library).getName();
			std::cout << std::setw( 10 ) << (*library).getStringVersion();
			std::cout << std::setw( 30 ) << (*library).getLicense();
			std::cout << std::endl;
		}
		return 0;
	}

	mediacore::NoDisplayProgress p;

	for( size_t argIndex = 1; argIndex < argc; ++argIndex )
	{
		try
		{
			mediaio::InputFile input( argv[argIndex] );
			input.analyse( p, mediaio::InputFile::eAnalyseLevelFull );

			// a simply metadata display
			std::cout << input;
		}
		catch( std::runtime_error& e )
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
		}
		catch( ... )
		{
			std::cerr << "ERROR: unknown error" << std::endl;
		}
	}


	return 0;
}
