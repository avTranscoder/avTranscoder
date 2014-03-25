#include <iostream>
#include <iomanip>

#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/InputStream.hpp>
#include <AvTranscoder/InputStreamAudio.hpp>

#include <AvTranscoder/OutputFile.hpp>

void transcodeAudio( const char* inputfilename, const char* outputFilename )
{
	using namespace avtranscoder;

	av_log_set_level( AV_LOG_FATAL );
	//av_log_set_level( AV_LOG_DEBUG );

	InputFile inputFile( inputfilename );
	inputFile.analyse();


	// init audio decoders
	InputStreamAudio inputStreamAudio( inputFile.getStream( 0 ) );

	OutputFile outputFile( outputFilename );

	outputFile.setup();


	AudioDesc audioDesc = inputFile.getStream( 0 ).getAudioDesc();

	outputFile.addAudioStream( audioDesc );

	AudioFrameDesc audioFrameDesc;
	AudioFrame sourceAudio( audioFrameDesc );

	DataStreamDesc dataDesc;
	DataStream data( dataDesc );

	// Encodage/transcodage
	std::cout << "start transcoding" << std::endl;

	size_t frame = 0;

	while( inputFile.getStream( 0 ).readNextPacket( data ) )
	{
		std::cout << "\rprocess frame " << frame << std::flush;

		outputFile.wrap( data, 0 );

		++frame;
	}
	std::cout << std::endl;

}

int main( int argc, char** argv )
{
	if( argc <= 2 )
	{
		std::cout << "audiorewrapper require medias filename" << std::endl;
		return( -1 );
	}

	std::cout << "start ..." << std::endl;
 
	transcodeAudio( argv[1], argv[2] );

	std::cout << "end ..." << std::endl;
}