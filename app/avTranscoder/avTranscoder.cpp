#include <iostream>
#include <iomanip>

#include <AvTranscoder/Media.hpp>
#include <AvTranscoder/InputStream.hpp>
#include <AvTranscoder/InputStreamAudio.hpp>
#include <AvTranscoder/InputStreamVideo.hpp>
#include <AvTranscoder/OutputStream.hpp>
#include <AvTranscoder/OutputStreamAudio.hpp>
#include <AvTranscoder/OutputStreamVideo.hpp>
#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/OutputFile.hpp>

#include <AvTranscoder/ColorTransform.hpp>

#include <AvTranscoder/DatasStructures/VideoDesc.hpp>
#include <AvTranscoder/DatasStructures/Image.hpp>

void displayMetadatas( const char* filename )
{
	using namespace avtranscoder;

	Media input( filename );
	input.analyse();
	std::cout << "format name              : " << input.getProperties().formatName << std::endl;
	std::cout << "format long name         : " << input.getProperties().formatLongName << std::endl;
	std::cout << "start time               : " << input.getProperties().startTime << std::endl;
	std::cout << "duration                 : " << input.getProperties().duration << std::endl;
	std::cout << "bitrate                  : " << input.getProperties().bitRate << std::endl;
	std::cout << "number of streams        : " << input.getProperties().streamsCount << std::endl;
	std::cout << "number of programs       : " << input.getProperties().programsCount << std::endl;
	std::cout << "number of video streams  : " << input.getProperties().videoStreams.size() << std::endl;
	std::cout << "number of audio streams  : " << input.getProperties().audioStreams.size() << std::endl;

	// std::vector< std::pair< std::string, std::string > > metadatas
	std::cout << "---------- Metadatas ----------" << std::endl;
	for( size_t metadataIndex = 0; metadataIndex < input.getProperties().metadatas.size(); ++metadataIndex )
	{
		std::cout << std::left << std::setw( 24 ) << input.getProperties().metadatas.at(metadataIndex).first <<" : " << input.getProperties().metadatas.at(metadataIndex).second << std::endl;
	}

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
		std::cout << "pixel type               : " << input.getProperties().videoStreams.at(videoStreamIndex).pixelName << std::endl;
		
		std::cout << "bit rate                 : " << input.getProperties().videoStreams.at(videoStreamIndex).bitRate << std::endl;

		std::cout << "color transfert          : " << input.getProperties().videoStreams.at(videoStreamIndex).colorTransfert << std::endl;
		std::cout << "colorspace               : " << input.getProperties().videoStreams.at(videoStreamIndex).colorspace << std::endl;
		std::cout << "color range              : " << input.getProperties().videoStreams.at(videoStreamIndex).colorRange << std::endl;
		std::cout << "color primaries          : " << input.getProperties().videoStreams.at(videoStreamIndex).colorPrimaries << std::endl;
		std::cout << "chroma sample location   : " << input.getProperties().videoStreams.at(videoStreamIndex).chromaSampleLocation << std::endl;
		std::cout << "interlaced               : " << ( input.getProperties().videoStreams.at(videoStreamIndex).isInterlaced ? "True" : "False" ) << std::endl;
		std::cout << "top field first          : " << ( input.getProperties().videoStreams.at(videoStreamIndex).topFieldFirst ? "True" : "False" ) << std::endl;
		std::cout << "field order              : " << input.getProperties().videoStreams.at(videoStreamIndex).fieldOrder << std::endl;

		std::cout << "gop                      : ";
		for( size_t frameIndex = 0; frameIndex < input.getProperties().videoStreams.at(videoStreamIndex).gopStructure.size(); ++frameIndex )
		{
			std::cout << input.getProperties().videoStreams.at(videoStreamIndex).gopStructure.at( frameIndex ).first;
			std::cout << ( input.getProperties().videoStreams.at(videoStreamIndex).gopStructure.at( frameIndex ).second ? "*" : " " );
		}
		std::cout << std::endl;
	}
	for( size_t audioStreamIndex = 0; audioStreamIndex < input.getProperties().audioStreams.size(); ++audioStreamIndex )
	{
		std::cout << "---------- Audio stream " << audioStreamIndex << " ----------" << std::endl;
		std::cout << "codec name               : " << input.getProperties().audioStreams.at(audioStreamIndex).codecName << std::endl;
		std::cout << "codec long name          : " << input.getProperties().audioStreams.at(audioStreamIndex).codecLongName << std::endl;
		std::cout << "sample format            : " << input.getProperties().audioStreams.at(audioStreamIndex).sampleFormat << std::endl;
		std::cout << "codec id                 : " << input.getProperties().audioStreams.at(audioStreamIndex).codecId << std::endl;
		std::cout << "stream id                : " << input.getProperties().audioStreams.at(audioStreamIndex).streamId << std::endl;
		std::cout << "sample rate              : " << input.getProperties().audioStreams.at(audioStreamIndex).sampleRate << std::endl;
		std::cout << "channels                 : " << input.getProperties().audioStreams.at(audioStreamIndex).channels << std::endl;
		std::cout << "bit rate                 : " << input.getProperties().audioStreams.at(audioStreamIndex).bit_rate << std::endl;
	}
}

void transcodeVideo( const char* inputfilename, const char* outputFilename )
{
	using namespace avtranscoder;

	Media input( inputfilename );
	input.analyse();

	// init video decoders
	InputStreamVideo inputStreamVideo; // take the first video stream per default

	av_log_set_level( AV_LOG_FATAL );
	av_log_set_level( AV_LOG_DEBUG );
	

	inputStreamVideo.setup( inputfilename, 0 );

	//dVideo.set( key, value );

	// same as
	//DecoderVideo dVideo( "inputFilename.mov", 1 );

	// init audio decoders
	InputStreamAudio isAudioLeft ( "inputFilename.wav" ); // take the first audio stream per default
	InputStreamAudio isAudioRight( "inputFilename.wav", 2 );

	// init video & audio encoders
	OutputStreamVideo outputStreamVideo;

	VideoDesc& videoDesc = outputStreamVideo.getVideoDesc();

	Pixel oPixel;

	oPixel.setSubsampling( eSubsampling422 );
	oPixel.setBitsPerPixel( 16 );

	ImageDesc imageDesc;
	imageDesc.setWidth ( input.getProperties().videoStreams.at(0).width );
	imageDesc.setHeight( input.getProperties().videoStreams.at(0).height );
	imageDesc.setPixel ( oPixel );

	Image sourceImage( imageDesc );

	videoDesc.setVideoCodec( "mpeg2video" );
	videoDesc.set( "b",50000000 );
	try
	{
		videoDesc.set( "unknownParameter", 120000000 );
	}
	catch( const std::exception& e )
	{
		std::cout << "[ERROR] " << e.what() << std::endl;
	}

	videoDesc.setTimeBase( 1, 25 ); // 25 fps

	videoDesc.setImageParameters( sourceImage );

	//videoDesc.initCodecContext();

	if( !outputStreamVideo.setup( ) )
	{
		std::cout << "error during initialising video output stream" << std::endl;
		exit( -1 );
	}

	DataStreamDesc dataStreamDesc;

	Image imageToEncode( sourceImage );
	DataStream codedImage( dataStreamDesc );


	OutputStreamAudio osAudioLeft ( );  // "AudioStreamEncoder" / "AudioOutputStream" ?
	OutputStreamAudio osAudioRight( );
	OutputStreamAudio osAudioLfe  ( );

	// setup wrapper
	OutputFile of( outputFilename );  // "Format" ? to keep libav naming


	if( ! of.setup( ) )
	{
		std::cout << "error during setup output file" << std::endl;
		exit( -1 );
	}

	InputFile inputFile;
	inputFile.setup( inputfilename );

	of.addVideoStream( inputFile.getVideoDesc( 0 ) );
	/*of.addAudioStream();
	of.addAudioStream();
	of.addAudioStream();
	of.addAudioStream();*/


	/*eVideoLeft  = wrapper.createVideoEncoder( );
	eVideoRight = wrapper.createVideoEncoder( dVideo );

	eAudioLeft = wrapper.createAudioEncoder( 2 );


	wrapper.createAudioEncoder( eAudioLeft, 2 );*/

	ColorTransform ct;


	// Encodage/transcodage
	std::cout << "start transcoding" << std::endl;

	size_t frame = 0;

	while( inputStreamVideo.readNextFrame( sourceImage ) )
	{
		std::cout << "\rprocess frame " << frame << std::flush;

		ct.convert( sourceImage, imageToEncode );

		outputStreamVideo.encodeFrame( imageToEncode, codedImage );

		of.wrap( codedImage, 0 );

		++frame;
	}
	std::cout << std::endl;
}

int main( int argc, char** argv )
{
	if( argc != 2 )
	{
		std::cout << "av++ require a media filename" << std::endl;
		return( -1 );
	}

	std::cout << "start ..." << std::endl;

	// a simply metadata getter
	displayMetadatas( argv[1] );

	// example of video Transcoding
	transcodeVideo( argv[1], "transcodedVideo.mxf" );

	std::cout << "end ..." << std::endl;


	// TESTS TO DO

	// audio -> audio

	// video -> video

	// Audio + video -> Audio + video

	// Audio // Video -> Audio + video

	// Audio * N // Video -> Audio *N + video

	// Audio * N // Video * N -> Audio *N + video * N


}
