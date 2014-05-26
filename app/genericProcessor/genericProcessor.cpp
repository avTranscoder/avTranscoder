
#include <AvTranscoder/Transcoder.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>


void parseConfigFile( const std::string& configFilename, avtranscoder::Transcoder::StreamsDefinition& streams )
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
			if( std::getline( is_line, streamId ) )
			{
				std::cout << filename << " ( " << streamId <<  " )" << std::endl;
				streams.push_back( std::pair< std::string, int >( filename, atoi( streamId.c_str() ) ) );
			}
		}
	}

	configFile.close();
}

int main( int argc, char** argv )
{
	if( argc != 3 )
	{
		std::cout << "avprocessor require an input config file and an output media filename" << std::endl;
		return( -1 );
	}

	av_log_set_level( AV_LOG_FATAL );

	try
	{
		std::cout << "start ..." << std::endl;

		std::string inputConfigFile( argv[1] );
		avtranscoder::OutputFile outputFile( argv[2] );

		avtranscoder::Transcoder::StreamsDefinition streams;

		parseConfigFile( inputConfigFile, streams );

		avtranscoder::Transcoder transcoder( outputFile );

		transcoder.add( streams );

		std::cout << "start Transcode" << std::endl;

		avtranscoder::ProgressListener progress;

		// video re-wrapping or transcoding if necessary
		transcoder.process( progress );

		std::cout << std::endl << "end ..." << std::endl;
	}
	catch( std::exception& e )
	{
		std::cerr << "ERROR: during process, an error occured:" << std::endl << e.what() << std::endl;
	}

}
