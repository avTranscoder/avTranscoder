#include <iostream>
#include <iomanip>
#include <cstdlib>

#include <avtranscoder/file/InputFile.hpp>
#include <avtranscoder/file/OutputFile.hpp>

#include <avtranscoder/frame/Frame.hpp>

#include <avtranscoder/essenceStream/AvInputAudio.hpp>
#include <avtranscoder/essenceStream/AvInputVideo.hpp>
#include <avtranscoder/essenceStream/AvOutputAudio.hpp>
#include <avtranscoder/essenceStream/AvOutputVideo.hpp>

#include <avtranscoder/transform/VideoTransform.hpp>

#include <avtranscoder/progress/ConsoleProgress.hpp>

void transcodeVideo( const char* inputfilename, const char* outputFilename )
{
	using namespace avtranscoder;

	// av_log_set_level( AV_LOG_DEBUG );

	Profile profile( true );
	ConsoleProgress p;

	InputFile input( inputfilename );
	input.analyse( p );

	input.readStream( input.getProperties().videoStreams.at( 0 ).streamId );

	// init video decoders
	AvInputVideo inputVideo( input.getStream( 0 ) );
	VideoFrameDesc VideoFrameDesc = input.getStream( 0 ).getVideoCodec().getVideoFrameDesc();
	VideoFrame sourceImage( VideoFrameDesc );

	// init video encoder
	AvOutputVideo outputVideo;
	outputVideo.setProfile( profile.getProfile( "xdcamhd422" ), VideoFrameDesc );
	VideoFrame imageToEncode( outputVideo.getVideoCodec().getVideoFrameDesc() );
	
	CodedData codedImage;

	// setup wrapper
	//mxftkwrapper::MxftkOutputFile of( outputFilename );
	OutputFile of( outputFilename );


	if( ! of.setup( ) )
	{
		std::cout << "error during setup output file" << std::endl;
		exit( -1 );
	}

	of.addVideoStream( outputVideo.getVideoCodec() );

	of.beginWrap();

	VideoTransform ct;


	// Encodage/transcodage
	std::cout << "start transcoding" << std::endl;

	size_t frame = 1;

	while( inputVideo.readNextFrame( sourceImage ) )
	{
		std::cout << "\rprocess frame " << frame << std::flush;
		
		ct.convert( sourceImage, imageToEncode );

		if( outputVideo.encodeFrame( imageToEncode, codedImage ) )
			of.wrap( codedImage, 0 );

		++frame;
	}

	while( outputVideo.encodeFrame( codedImage ) )
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
