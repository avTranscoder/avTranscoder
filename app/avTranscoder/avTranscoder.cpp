#include <iostream>
#include <iomanip>
#include <cstdlib>

#include <AvTranscoder/AvInputStream.hpp>
#include <AvTranscoder/InputStreamAudio.hpp>
#include <AvTranscoder/InputStreamVideo.hpp>
#include <AvTranscoder/OutputStream.hpp>
#include <AvTranscoder/OutputStreamAudio.hpp>
#include <AvTranscoder/OutputStreamVideo.hpp>

#include <AvTranscoder/File/InputFile.hpp>
#include <AvTranscoder/File/OutputFile.hpp>

#include <AvTranscoder/ColorTransform.hpp>

#include <AvTranscoder/DatasStructures/VideoDesc.hpp>
#include <AvTranscoder/DatasStructures/Image.hpp>

void transcodeVideo( const char* inputfilename, const char* outputFilename )
{
	using namespace avtranscoder;

	// av_log_set_level( AV_LOG_DEBUG );

	ProgressListener p;

	InputFile input( inputfilename );
	input.analyse( p );

	input.readStream( input.getProperties().videoStreams.at( 0 ).streamId );

	// init video decoders
	InputStreamVideo inputStreamVideo( input.getStream( 0 ) );
	Image sourceImage( input.getStream( 0 ).getVideoDesc().getImageDesc() );

	// init video encoder
	OutputStreamVideo outputStreamVideo;
	outputStreamVideo.setProfile( "xdcamhd422" );
	Image imageToEncode( outputStreamVideo.getVideoDesc().getImageDesc() );
	
	DataStream codedImage;

	// setup wrapper
	//mxftkwrapper::MxftkOutputFile of( outputFilename );
	OutputFile of( outputFilename );


	if( ! of.setup( ) )
	{
		std::cout << "error during setup output file" << std::endl;
		exit( -1 );
	}

	of.addVideoStream( outputStreamVideo.getVideoDesc() );

	of.beginWrap();

	ColorTransform ct;


	// Encodage/transcodage
	std::cout << "start transcoding" << std::endl;

	size_t frame = 1;

	while( inputStreamVideo.readNextFrame( sourceImage ) )
	{
		std::cout << "\rprocess frame " << frame << std::flush;
		
		ct.convert( sourceImage, imageToEncode );

		if( outputStreamVideo.encodeFrame( imageToEncode, codedImage ) )
			of.wrap( codedImage, 0 );

		++frame;
	}

	while( outputStreamVideo.encodeFrame( codedImage ) )
	{
		of.wrap( codedImage, 0 );
	}

	of.endWrap();
	std::cout << std::endl;
}

int main( int argc, char** argv )
{
	if( argc != 3 )
	{
		std::cout << "av++ require a media filename" << std::endl;
		return( -1 );
	}

	std::cout << "start ..." << std::endl;

	try
	{
		transcodeVideo( argv[1], argv[2] );
	}
	catch( std::exception &e )
	{
		std::cout << "[ERROR] " << e.what() << std::endl;
	}


	std::cout << "end ..." << std::endl;
}
