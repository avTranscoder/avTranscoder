#include <AvTranscoder/transcoder/Transcoder.hpp>
#include <AvTranscoder/file/OutputFile.hpp>

static std::string outputFileName = "thumbnail.jpg";

int main( int argc, char** argv )
{
	std::string help;
	help += "Usage\n";
	help += "\tavthumbnail INPUT_FILE_NAME FRAME [OUTPUT_FILE_NAME] [--help]\n";
	help += "Command line options\n";
	help += "\tOUTPUT_FILE_NAME: name of the output file (thumbnail.jpg by default)\n";
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
	}
	if( argc > 3 )
	{
		outputFileName = arguments.at( 2 );
	}

	// Check required arguments
	if( argc < 3 )
	{
		std::cout << "avthumbnail creates a thumbnail from an input file at an indicated frame." << std::endl;
		std::cout << "Use option --help to display help" << std::endl; 
		return( -1 );
	}

	avtranscoder::setLogLevel( AV_LOG_QUIET );
	avtranscoder::preloadCodecsAndFormats();

	// input file
	std::string inputFileName( argv[1] );
	avtranscoder::InputFile intputFile( inputFileName );
	intputFile.seekAtFrame( atoi( argv[2] ) );

	// output file
	avtranscoder::ProfileLoader::Profile formatProfile;
	formatProfile[ avtranscoder::constants::avProfileIdentificator ] = "thumbnailFormatPreset";
	formatProfile[ avtranscoder::constants::avProfileIdentificatorHuman ] = "Thumbnail format preset";
	formatProfile[ avtranscoder::constants::avProfileType ] = avtranscoder::constants::avProfileTypeFormat;
	formatProfile[ avtranscoder::constants::avProfileFormat ] = "mjpeg";
	avtranscoder::OutputFile outputFile( outputFileName );
	outputFile.setProfile( formatProfile );

	// input stream
	avtranscoder::InputStream inputStream( intputFile, 0 );
	inputStream.activate();

	// output stream
	avtranscoder::ProfileLoader profileLoader( true );
	avtranscoder::StreamTranscoder outputStream( inputStream, outputFile, profileLoader.getProfile( "mjpeg" ) );

	// transcoder
	avtranscoder::Transcoder transcoder( outputFile );
	transcoder.add( outputStream );

	// process
	outputFile.beginWrap();
	transcoder.preProcessCodecLatency();
	transcoder.processFrame();
	outputFile.endWrap();
}
