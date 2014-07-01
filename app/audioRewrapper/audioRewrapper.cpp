#include <iostream>
#include <iomanip>

#include <AvTranscoder/File/InputFile.hpp>
#include <AvTranscoder/File/OutputFile.hpp>

#include <AvTranscoder/EssenceStream/InputAudio.hpp>
#include <AvTranscoder/EssenceStream/OutputAudio.hpp>

#include <AvTranscoder/EssenceTransform/AudioEssenceTransform.hpp>

void rewrapAudio( const char* inputfilename, const char* outputFilename )
{
	using namespace avtranscoder;

	av_log_set_level( AV_LOG_FATAL );
	av_log_set_level( AV_LOG_DEBUG );

	ProgressListener p;

	InputFile inputFile( inputfilename );
	inputFile.analyse( p );

	size_t audioStreamId = inputFile.getProperties().audioStreams.at( 0 ).streamId;

	inputFile.readStream( audioStreamId );

	OutputFile outputFile( outputFilename );

	outputFile.setup();

	OutputStream& audioStream = outputFile.addAudioStream( inputFile.getStream( audioStreamId ).getAudioDesc() );
	
	outputFile.beginWrap();

	DataStream data;

	// Encodage/transcodage
	std::cout << "start re-wrapping" << std::endl;

	size_t frame = 0;

	while( inputFile.getStream( audioStreamId ).readNextPacket( data ) )
	{
		std::cout << "\rprocess frame " << (int)frame - 1 << std::flush;

		audioStream.wrap( data );

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

	ProgressListener p;

	InputFile inputFile( inputfilename );
	inputFile.analyse( p );

	OutputFile outputFile( outputFilename );
	outputFile.setup();
	
	// init audio decoders
	size_t audioStreamId = inputFile.getProperties().audioStreams.at( 0 ).streamId;
	InputAudio inputAudio( inputFile.getStream( audioStreamId ) );
	inputFile.readStream( audioStreamId );

	// init audio encoders
	OutputAudio outputAudio;
	AudioDesc& audioOutputDesc = outputAudio.getAudioDesc();
	audioOutputDesc.setAudioCodec( "pcm_s24le" );
	audioOutputDesc.setAudioParameters( 
		inputFile.getStream( 0 ).getAudioDesc().getSampleRate(),
		inputFile.getStream( 0 ).getAudioDesc().getChannels(),
		AV_SAMPLE_FMT_S16//,inputFile.getStream( 0 ).getAudioDesc().getSampleFormat()
		);
	
	if( ! outputAudio.setup( ) )
	{
		throw std::runtime_error( "error during initialising audio output stream" );
	}
	
	OutputStream& audioStream = outputFile.addAudioStream( audioOutputDesc );
	outputFile.beginWrap();
	
	// init convert
	AudioEssenceTransform audioEssenceTransform;
	
	DataStream codedFrame;

	// start transcoding process
	std::cout << "start transcoding" << std::endl;
	
	AudioFrame audioFrameSource( inputFile.getStream( 0 ).getAudioDesc().getFrameDesc() );
	AudioFrame audioFrameToEncode( audioOutputDesc.getFrameDesc() );
	
	size_t frame = 0;
	while( inputAudio.readNextFrame( audioFrameSource ) )
	{
		std::cout << "\rprocess frame " << (int)frame - 1 << std::flush;

		audioEssenceTransform.convert( audioFrameSource, audioFrameToEncode );
		
		outputAudio.encodeFrame( audioFrameToEncode, codedFrame );
				
		audioStream.wrap( codedFrame );

		++frame;
		// if you want to stop the transcoding process (after 10s at 48,1 KHz)
//		if(frame == 10*48100)
//			break;
	}
	std::cout << std::endl;
	
	outputAudio.encodeFrame( codedFrame );
	audioStream.wrap( codedFrame );
	
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
		//transcodeAudio( argv[1], argv[2] );
	}
	catch( std::exception &e )
	{
		std::cout << "[ERROR] " << e.what() << std::endl;
	}
	std::cout << "end ..." << std::endl;
}
