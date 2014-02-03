#include <iostream>

#include <AvTranscoder/Media.hpp>
#include <AvTranscoder/InputStream.hpp>
#include <AvTranscoder/InputStreamAudio.hpp>
#include <AvTranscoder/InputStreamVideo.hpp>
#include <AvTranscoder/OutputStream.hpp>
#include <AvTranscoder/OutputStreamAudio.hpp>
#include <AvTranscoder/OutputStreamVideo.hpp>
#include <AvTranscoder/OutputFile.hpp>

int main( int argc, char** argv )
{
	using namespace avtranscoder;
	if( argc != 2 )
	{
		std::cout << "av++ require a media filename" << std::endl;
		return( -1 );
	}

	std::cout << "start ..." << std::endl;


	// a simply metadata getter
	Media input( argv[1] );
	input.analyse();
	std::cout << "format name              : " << input.getProperties().formatName << std::endl;
	std::cout << "format long name         : " << input.getProperties().formatLongName << std::endl;
	std::cout << "start time               : " << input.getProperties().startTime << std::endl;
	std::cout << "duration                 : " << input.getProperties().duration << std::endl;
	std::cout << "bitrate                  : " << input.getProperties().bitRate << std::endl;
	std::cout << "number of streams        : " << input.getProperties().streamsCount << std::endl;
	std::cout << "number of video streams  : " << input.getProperties().videoStreams.size() << std::endl;
	std::cout << "number of audio streams  : " << input.getProperties().audioStreams.size() << std::endl;
	

	for( size_t videoStreamIndex = 0; videoStreamIndex < input.getProperties().videoStreams.size(); ++videoStreamIndex )
	{
		std::cout << "---------- Video stream " << videoStreamIndex << " ----------" << std::endl;
		std::cout << "codec name               : " << input.getProperties().videoStreams.at(videoStreamIndex).codecName << std::endl;
		std::cout << "codec long name          : " << input.getProperties().videoStreams.at(videoStreamIndex).codecLongName << std::endl;
		std::cout << "profile name             : " << input.getProperties().videoStreams.at(videoStreamIndex).profileName << std::endl;
		std::cout << "profile                  : " << input.getProperties().videoStreams.at(videoStreamIndex).profile << std::endl;
		std::cout << "level                    : " << input.getProperties().videoStreams.at(videoStreamIndex).level << std::endl;
		std::cout << "width                    : " << input.getProperties().videoStreams.at(videoStreamIndex).width << std::endl;
		std::cout << "height                   : " << input.getProperties().videoStreams.at(videoStreamIndex).height << std::endl;
		std::cout << "pixel aspect ratio       : " << input.getProperties().videoStreams.at(videoStreamIndex).sar.num << "/" <<
		                                              input.getProperties().videoStreams.at(videoStreamIndex).sar.den << std::endl;
		std::cout << "display aspect ratio     : " << input.getProperties().videoStreams.at(videoStreamIndex).dar.num << "/" <<
		                                              input.getProperties().videoStreams.at(videoStreamIndex).dar.den << std::endl;

		std::cout << "gop                      : ";
		for( size_t frameIndex = 0; frameIndex < input.getProperties().videoStreams.at(videoStreamIndex).gopStructure.size(); ++frameIndex )
		{
			std::cout << input.getProperties().videoStreams.at(videoStreamIndex).gopStructure.at( frameIndex ).first;
			std::cout << ( input.getProperties().videoStreams.at(videoStreamIndex).gopStructure.at( frameIndex ).second ? "*" : " " );
		}
		std::cout << std::endl;

	}

	// init video decoders
	InputStreamVideo isVideo; // take the first video stream per default

	if( !isVideo.setup( argv[1], 0 ) )
	{
		std::cout << "error during initialising video input reader" << std::endl;
		return( -1 );
	}

	std::cout << "Input Video Stream Properties " << std::endl;
	std::cout << "size 	"      << isVideo.getWidth() << "x" << isVideo.getHeight() << std::endl;
	std::cout << "components " << isVideo.getComponents() << std::endl;
	std::cout << "bit depth  " << isVideo.getBitDepth() << std::endl;
	//dVideo.set( key, value );

	// same as
	//DecoderVideo dVideo( "inputFilename.mov", 1 );

	// init audio decoders
	InputStreamAudio isAudioLeft ( "inputFilename.wav" ); // take the first audio stream per default
	InputStreamAudio isAudioRight( "inputFilename.wav", 2 );

	// init video & audio encoders
	OutputStreamVideo osVideo;

	osVideo.setWidth( isVideo.getWidth() );
	osVideo.setHeight( isVideo.getHeight() );
	osVideo.setComponents( isVideo.getComponents() );
	osVideo.setBitDepth( isVideo.getBitDepth() );
	//eVideo.set( "mv_method", "me_hex" );
	
	if( !osVideo.setup( ) )
	{
		std::cout << "error during initialising video output stream" << std::endl;
		return( -1 );
	}


	OutputStreamAudio osAudioLeft ( );  // "AudioStreamEncoder" / "AudioOutputStream" ?
	OutputStreamAudio osAudioRight( );
	OutputStreamAudio osAudioLfe  ( );

	// setup wrapper
	OutputFile of( "codedFilename.mxf" );  // "Format" ? to keep libav naming

	if( ! of.setup() )
	{
		std::cout << "error during setup output file" << std::endl;
		return( -1 );
	}

	if( ! of.addVideoStream() )
	{
		std::cout << "error during adding output video stream" << std::endl;
		return( -1 );
	}
	/*of.addAudioStream();
	of.addAudioStream();
	of.addAudioStream();
	of.addAudioStream();*/


	/*eVideoLeft  = wrapper.createVideoEncoder( );
	eVideoRight = wrapper.createVideoEncoder( dVideo );

	eAudioLeft = wrapper.createAudioEncoder( 2 );


	wrapper.createAudioEncoder( eAudioLeft, 2 );*/


	// Encodage/transcodage

	std::vector<unsigned char> frameBuffer;
	std::vector<unsigned char> sourceImage( isVideo.getWidth() * isVideo.getHeight() * 3, 120 );

	std::vector<unsigned char> codedImage;

	for( size_t count = 0; count < 10; ++count )
	{
		isVideo.readNextFrame( frameBuffer );

		osVideo.encodeFrame( frameBuffer, codedImage );
		//std::cout << "decoded size " << frameBuffer.size() << " encode frame " << count << " size " << codedImage.size() << std::endl;

		of.wrap( codedImage, 0 );
	}


	//for( size_t frame : dVideo.framesCount() )
	{
		// read & decode source image,
		// send it to encoder which wraps it automatically
		//eVideo.encode( /* raw data */ dVideo.readNextFrame() );
		//eVideo.write()

		// also to access to decoded data image
		//dVideo.getDataFrame();


		//wrapper.write( /* char */ myRawData, streamIndex );
	}

	//eAudioLeft.encode(  );

	std::cout << "end ..." << std::endl;


	// TESTS

	// audio -> audio

	// video -> video

	// Audio + video -> Audio + video

	// Audio // Video -> Audio + video

	// Audio * N // Video -> Audio *N + video

	// Audio * N // Video * N -> Audio *N + video * N

}
