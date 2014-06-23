#include <iostream>
#include <iomanip>

#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/OutputFile.hpp>

#include <AvTranscoder/AvInputStream.hpp>
#include <AvTranscoder/InputStreamAudio.hpp>
#include <AvTranscoder/OutputStreamAudio.hpp>

#include <AvTranscoder/AudioTransform.hpp>



void rewrapAudio( const char* inputfilename, const char* outputFilename )
{
	using namespace avtranscoder;

	av_log_set_level( AV_LOG_FATAL );
	av_log_set_level( AV_LOG_DEBUG );

	InputFile inputFile( inputfilename );
	inputFile.analyse();
	inputFile.readStream( 0 );

	OutputFile outputFile( outputFilename );

	outputFile.setup();

	outputFile.addAudioStream( inputFile.getStream( 0 ).getAudioDesc() );
	
	outputFile.beginWrap();

	DataStream data;

	// Encodage/transcodage
	std::cout << "start re-wrapping" << std::endl;

	size_t frame = 0;

	while( inputFile.getStream( 0 ).readNextPacket( data ) )
	{
		std::cout << "\rprocess frame " << (int)frame - 1 << std::flush;

		outputFile.wrap( data, 0 );

		++frame;
	}
	outputFile.endWrap();
	std::cout << std::endl;
}

void transcodeAudio( const char* inputfilename, const char* outputFilename )
{
	using namespace avtranscoder;

	av_log_set_level( AV_LOG_FATAL );
	av_log_set_level( AV_LOG_DEBUG );

	InputFile inputFile( inputfilename );
	inputFile.analyse();

	OutputFile outputFile( outputFilename );
	outputFile.setup();
	
	// init audio decoders
	InputStreamAudio inputStreamAudio( inputFile.getStream( 0 ) );
	inputFile.getStream( 0 ).setBufferred( true );

	// init audio encoders
	OutputStreamAudio outputStreamAudio;
	AudioDesc& audioOutputDesc = outputStreamAudio.getAudioDesc();
	audioOutputDesc.setAudioCodec( "pcm_s24le" );
	audioOutputDesc.setAudioParameters( 
		inputFile.getStream( 0 ).getAudioDesc().getSampleRate(),
		inputFile.getStream( 0 ).getAudioDesc().getChannels(),
		AV_SAMPLE_FMT_S32//,inputFile.getStream( 0 ).getAudioDesc().getSampleFormat()
		);
	
	if( ! outputStreamAudio.setup( ) )
	{
		throw std::runtime_error( "error during initialising audio output stream" );
	}
	
	outputFile.addAudioStream( audioOutputDesc );
	outputFile.beginWrap();
	
	// init convert
	AudioTransform audioTransform;
	
	DataStream codedFrame;

	// start transcoding process
	std::cout << "start transcoding" << std::endl;
	
	AudioFrame audioFrameSource( inputFile.getStream( 0 ).getAudioDesc().getFrameDesc() );
	AudioFrame audioFrameToEncode( audioOutputDesc.getFrameDesc() );
	
	size_t frame = 0;
	while( inputStreamAudio.readNextFrame( audioFrameSource ) )
	{
		std::cout << "\rprocess frame " << (int)frame - 1 << std::flush;

		audioTransform.convert( audioFrameSource, audioFrameToEncode );
		
		outputStreamAudio.encodeFrame( audioFrameToEncode, codedFrame );
				
		outputFile.wrap( codedFrame, 0 );

		++frame;
		// if you want to stop the transcoding process (after 10s at 48,1 KHz)
//		if(frame == 10*48100)
//			break;
	}
	std::cout << std::endl;
	
	outputStreamAudio.encodeFrame( codedFrame );
	outputFile.wrap( codedFrame, 0 );
	
	// end of transcoding process
	
	outputFile.endWrap();
}

int main( int argc, char** argv )
{
	if( argc <= 2 )
	{
		std::cout << "audiorewrapper require medias filename" << std::endl;
		std::cout << "example: audioWrap inputfile.ext outputfile.ext" << std::endl;
		return( -1 );
	}

	std::cout << "start ..." << std::endl;

	try
	{
		rewrapAudio( argv[1], argv[2] );
		// transcodeAudio( argv[1], argv[2] );
	}
	catch( std::exception &e )
	{
		std::cout << "[ERROR] " << e.what() << std::endl;
	}
	std::cout << "end ..." << std::endl;
}
