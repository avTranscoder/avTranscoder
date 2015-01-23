#include <AvTranscoder/transcoder/Transcoder.hpp>
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
static bool useAudioGenerator = false;

bool verbose = false;

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
	if( argc < 3 )
	{
		std::cout << "avprocessor require an input config file and an output media filename" << std::endl;
		return( -1 );
	}

	std::vector< std::string > arguments;
	for( int argument = 1; argument < argc; ++argument )
	{
		arguments.push_back( argv[argument] );
	}
	for( size_t argument = 0; argument < arguments.size(); ++argument )
	{
		if( arguments.at( argument ) == "--generate-black" )
		{
			useVideoGenerator = true;
		}
		else if( arguments.at( argument ) == "--generate-silence" )
		{
			useAudioGenerator = true;
		}
	}

	av_log_set_level( AV_LOG_FATAL );
	if( verbose )
		av_log_set_level( AV_LOG_DEBUG );

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
		transcoder.setProcessMethod( avtranscoder::eProcessMethodLongest );
		
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
