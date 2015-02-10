#include <AvTranscoder/transcoder/Transcoder.hpp>
#include <AvTranscoder/file/OutputFile.hpp>
#include <AvTranscoder/progress/ConsoleProgress.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>

static const size_t dummyWidth = 1920;
static const size_t dummyHeight = 1080;
static const std::string dummyPixelFormat = "yuv420p";
static const std::string dummyVideoCodec = "mpeg2video";

static const size_t dummySampleRate = 48000;
static const size_t dummyChannels = 1;
static const std::string dummySampleFormat = "s16";
static const std::string dummyAudioCodec = "pcm_s16le";

static bool useVideoGenerator = false;

static bool verbose = false;

void parseConfigFile( const std::string& configFilename, avtranscoder::Transcoder& transcoder )
{
	std::ifstream configFile( configFilename.c_str(), std::ifstream::in );

	std::string line;
	while( std::getline( configFile, line ) )
	{
		std::istringstream is_line( line );
		std::string filename;
		if( std::getline( is_line, filename, '=' ) )
		{
			std::string streamId;
			if( std::getline( is_line, streamId, ':' ) )
			{
				std::string transcodeProfile;
				std::getline( is_line, transcodeProfile );

				std::stringstream ss( streamId );
				size_t streamIndex = 0;
				char separator;
				int subStreamIndex = -1;
				ss >> streamIndex;
				ss >> separator;
				if( separator == '.' )
					ss >> subStreamIndex;

				if( verbose )
				{
					std::cout << ( filename.length() ? filename : "dummy stream" );
					std::cout << " ( " << streamIndex;
					if( subStreamIndex > -1 )
						std::cout << " | " << subStreamIndex << " ";
					std::cout << " ) : ";
					std::cout << ( transcodeProfile.length() ? transcodeProfile : "rewrap" );
					std::cout << std::endl;
				}
				
				// dummy stream, need a ICodec (audio or video)
				if( ! filename.length() )
				{
					if( useVideoGenerator )
					{
						// video
						avtranscoder::VideoCodec inputCodec( avtranscoder::eCodecTypeEncoder, dummyVideoCodec );
						avtranscoder::VideoFrameDesc imageDesc( dummyWidth, dummyHeight, dummyPixelFormat );
						inputCodec.setImageParameters( imageDesc );

						transcoder.add( filename, streamIndex, subStreamIndex, transcodeProfile, inputCodec );
					}
					else
					{
						// audio
						avtranscoder::AudioCodec inputCodec( avtranscoder::eCodecTypeEncoder, dummyAudioCodec );
						avtranscoder::AudioFrameDesc audioDesc( dummySampleRate, dummyChannels, dummySampleFormat );
						inputCodec.setAudioParameters( audioDesc );

						transcoder.add( filename, streamIndex, subStreamIndex, transcodeProfile, inputCodec );
					}
				}
				else
				{
					transcoder.add( filename, streamIndex, subStreamIndex, transcodeProfile );
				}
			}
		}
	}

	configFile.close();
}

int main( int argc, char** argv )
{
	std::string help;
	help += "Usage\n";
	help += "\tavprocessor CONFIG.TXT OUTPUT_FILE_NAME [--generate-black] [--verbose] [--help]\n";
	help += "CONFIG.TXT\n";
	help += "\tEach line will be one stream in the output.\n";
	help += "\tPattern of each line is:\n";
	help += "\t[inputFile]=STREAM_ID.[subStreamId]:[profileName]\n";
	help += "\tNo inputFile: will generate black image / audio silence (audio by default)\n";
	help += "\tNo subStreamId: will process of channels of the stream\n";
	help += "\tNo profileName: will rewrap the stream\n";
	help += "Command line options\n";
	help += "\t--generate-black: stream which not referred to an input, will generate an output video stream with black images (by default generate audio stream with silence)\n";
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
		if( arguments.at( argument ) == "--generate-black" )
		{
			useVideoGenerator = true;
		}
		if( arguments.at( argument ) == "--verbose" )
		{
			verbose = true;
		}
	}

	// Check required arguments
	if( argc < 3 )
	{
		std::cout << "avprocessor can rewrap or transcode inputs to create an output media file." << std::endl;
		std::cout << "Use option --help to display help" << std::endl; 
		return( -1 );
	}

	avtranscoder::setLogLevel( AV_LOG_QUIET );
	if( verbose )
		avtranscoder::setLogLevel( AV_LOG_DEBUG );

	try
	{
		if( verbose )
			std::cout << "start ..." << std::endl;
		avtranscoder::preloadCodecsAndFormats();

		if( verbose )
			std::cout << "output file: " << argv[2] << std::endl;

		std::string inputConfigFile( argv[1] );
		avtranscoder::OutputFile outputFile( argv[2] );

		avtranscoder::Transcoder transcoder( outputFile );

		if( verbose )
			std::cout << "parse config file" << std::endl;
		parseConfigFile( inputConfigFile, transcoder );

		// set verbose of all stream
		transcoder.setVerbose( verbose );
		
		if( verbose )
			std::cout << "start Transcode" << std::endl;

		avtranscoder::ConsoleProgress progress;
		transcoder.process( progress );

		std::cout << std::endl;
		if( verbose )
				std::cout << "end ..." << std::endl;
	}
	catch( std::exception& e )
	{
		std::cerr << "ERROR: during process, an error occured: " << e.what() << std::endl;
	}
	catch( ... )
	{
		std::cerr << "ERROR: during process, an unknown error occured" << std::endl;
	}

}
