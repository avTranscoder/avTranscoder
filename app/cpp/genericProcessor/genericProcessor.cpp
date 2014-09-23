
#include <AvTranscoder/Transcoder/Transcoder.hpp>

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
static const std::string dummyAudioCodec = "pcm_s16le";

// bool verbose = false;
bool verbose = true;

void parseConfigFile( const std::string& configFilename, avtranscoder::Transcoder& transcoder, avtranscoder::Profile& profile )
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
				
				// dummy stream, need a CodedDesc (audio or video)
				if( ! filename.length() )
				{
					// video
					avtranscoder::VideoFrameDesc imageDesc;
					imageDesc.setWidth( dummyWidth );
					imageDesc.setHeight( dummyHeight );
					imageDesc.setDar( dummyWidth, dummyHeight );
					avtranscoder::Pixel inputPixel( dummyPixelFormat );
					imageDesc.setPixel( inputPixel );

					avtranscoder::VideoDesc inputVideoDesc( dummyVideoCodec );
					inputVideoDesc.setImageParameters( imageDesc );
					
					transcoder.add( filename, streamIndex, subStreamIndex, transcodeProfile, inputVideoDesc );
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
	if( argc != 3 )
	{
		std::cout << "avprocessor require an input config file and an output media filename" << std::endl;
		return( -1 );
	}

	av_log_set_level( AV_LOG_FATAL );

	if( verbose )
		av_log_set_level( AV_LOG_DEBUG );

	try
	{
		if( verbose )
			std::cout << "start ..." << std::endl;

		avtranscoder::Profile profiles( true );

		if( verbose )
			std::cout << "output file: " << argv[2] << std::endl;

		std::string inputConfigFile( argv[1] );
		avtranscoder::OutputFile outputFile( argv[2] );

		avtranscoder::Transcoder transcoder( outputFile );

		if( verbose )
			std::cout << "parse config file" << std::endl;
		parseConfigFile( inputConfigFile, transcoder, profiles );

		// set verbose of all stream
		transcoder.setVerbose( verbose );
		transcoder.setProcessMethod( avtranscoder::eProcessMethodLongest );
		//transcoder.setOutputFps( 12 );
		transcoder.init();
		
		if( verbose )
			std::cout << "start Transcode" << std::endl;

		avtranscoder::ConsoleProgress progress;

		// video re-wrapping or transcoding if necessary
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
