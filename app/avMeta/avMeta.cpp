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

	std::string separator( "====================" );

	Media input( filename );
	input.analyse();
	std::cout << separator << " Wrapper " << separator << std::endl;
	std::cout << "filename                 : " << input.getProperties().filename << std::endl;
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
	std::cout << separator << " Metadatas " << separator << std::endl;
	for( size_t metadataIndex = 0; metadataIndex < input.getProperties().metadatas.size(); ++metadataIndex )
	{
		std::cout << std::left << std::setw( 24 ) << input.getProperties().metadatas.at(metadataIndex).first <<" : " << input.getProperties().metadatas.at(metadataIndex).second << std::endl;
	}

	for( size_t videoStreamIndex = 0; videoStreamIndex < input.getProperties().videoStreams.size(); ++videoStreamIndex )
	{
		std::cout << separator << " Video stream " << videoStreamIndex << " " << separator << std::endl;
		std::cout << "codec name               : " << input.getProperties().videoStreams.at(videoStreamIndex).codecName << std::endl;
		std::cout << "codec long name          : " << input.getProperties().videoStreams.at(videoStreamIndex).codecLongName << std::endl;
		std::cout << "codec id                 : " << input.getProperties().videoStreams.at(videoStreamIndex).codecId << std::endl;
		std::cout << "stream id                : " << input.getProperties().videoStreams.at(videoStreamIndex).streamId << std::endl;
		std::cout << "profile name             : " << input.getProperties().videoStreams.at(videoStreamIndex).profileName << std::endl;
		std::cout << "profile                  : " << input.getProperties().videoStreams.at(videoStreamIndex).profile << std::endl;
		std::cout << "level                    : " << input.getProperties().videoStreams.at(videoStreamIndex).level << std::endl;
		std::cout << "width                    : " << input.getProperties().videoStreams.at(videoStreamIndex).width << std::endl;
		std::cout << "height                   : " << input.getProperties().videoStreams.at(videoStreamIndex).height << std::endl;
		std::cout << "dtgActiveFormat          : " << input.getProperties().videoStreams.at(videoStreamIndex).dtgActiveFormat << std::endl;
		
		std::cout << "timeBase                 : " << input.getProperties().videoStreams.at(videoStreamIndex).timeBase.num << "/" <<
		                                              input.getProperties().videoStreams.at(videoStreamIndex).timeBase.den << std::endl;
		std::cout << "fps                      : " << input.getProperties().videoStreams.at(videoStreamIndex).fps << std::endl;
		std::cout << "ticksPerFrame            : " << input.getProperties().videoStreams.at(videoStreamIndex).ticksPerFrame << std::endl;

		std::cout << "pixel aspect ratio       : " << input.getProperties().videoStreams.at(videoStreamIndex).sar.num << "/" <<
		                                              input.getProperties().videoStreams.at(videoStreamIndex).sar.den << std::endl;
		std::cout << "display aspect ratio     : " << input.getProperties().videoStreams.at(videoStreamIndex).dar.num << "/" <<
		                                              input.getProperties().videoStreams.at(videoStreamIndex).dar.den << std::endl;
		std::cout << "pixel type               : " << input.getProperties().videoStreams.at(videoStreamIndex).pixelName << std::endl;
		std::cout << "bit wise acked           : " << ( input.getProperties().videoStreams.at(videoStreamIndex).bitWisePacked ? "True" : "False" ) << std::endl;
		std::cout << "rgb pixel                : " << ( input.getProperties().videoStreams.at(videoStreamIndex).rgbPixelData ? "True" : "False" ) << std::endl;
		std::cout << "as alpha                 : " << ( input.getProperties().videoStreams.at(videoStreamIndex).asAlpha ? "True" : "False" ) << std::endl;
		std::cout << "endianess                : " << input.getProperties().videoStreams.at(videoStreamIndex).endianess << std::endl;
		
		std::cout << "bit rate                 : " << input.getProperties().videoStreams.at(videoStreamIndex).bitRate << std::endl;

		std::cout << "color transfert          : " << input.getProperties().videoStreams.at(videoStreamIndex).colorTransfert << std::endl;
		std::cout << "colorspace               : " << input.getProperties().videoStreams.at(videoStreamIndex).colorspace << std::endl;
		std::cout << "color range              : " << input.getProperties().videoStreams.at(videoStreamIndex).colorRange << std::endl;
		std::cout << "color primaries          : " << input.getProperties().videoStreams.at(videoStreamIndex).colorPrimaries << std::endl;
		std::cout << "indexed colors           : " << ( input.getProperties().videoStreams.at(videoStreamIndex).indexedColors ? "True" : "False" ) << std::endl;
		std::cout << "pseudo paletted          : " << ( input.getProperties().videoStreams.at(videoStreamIndex).pseudoPaletted ? "True" : "False" ) << std::endl;
		std::cout << "components count         : " << input.getProperties().videoStreams.at(videoStreamIndex).componentsCount << std::endl;
		std::cout << "chroma width             : " << input.getProperties().videoStreams.at(videoStreamIndex).chromaWidth << std::endl;
		std::cout << "chroma height            : " << input.getProperties().videoStreams.at(videoStreamIndex).chromaHeight << std::endl;
		std::cout << "chroma sample location   : " << input.getProperties().videoStreams.at(videoStreamIndex).chromaSampleLocation << std::endl;
		std::cout << "interlaced               : " << ( input.getProperties().videoStreams.at(videoStreamIndex).isInterlaced ? "True" : "False" ) << std::endl;
		std::cout << "top field first          : " << ( input.getProperties().videoStreams.at(videoStreamIndex).topFieldFirst ? "True" : "False" ) << std::endl;
		std::cout << "field order              : " << input.getProperties().videoStreams.at(videoStreamIndex).fieldOrder << std::endl;
		std::cout << "gop size                 : " << input.getProperties().videoStreams.at(videoStreamIndex).gopSize << std::endl;
		std::cout << "has B frames             : " << ( input.getProperties().videoStreams.at(videoStreamIndex).hasBFrames ? "True" : "False" ) << std::endl;

		std::cout << "gop                      : ";
		for( size_t frameIndex = 0; frameIndex < input.getProperties().videoStreams.at(videoStreamIndex).gopStructure.size(); ++frameIndex )
		{
			std::cout << input.getProperties().videoStreams.at(videoStreamIndex).gopStructure.at( frameIndex ).first;
			std::cout << ( input.getProperties().videoStreams.at(videoStreamIndex).gopStructure.at( frameIndex ).second ? "*" : " " );
		}
		std::cout << std::endl;
		std::cout << "references frames        : " << input.getProperties().videoStreams.at(videoStreamIndex).referencesFrames << std::endl;

	}
	for( size_t audioStreamIndex = 0; audioStreamIndex < input.getProperties().audioStreams.size(); ++audioStreamIndex )
	{
		std::cout << separator << " Audio stream " << audioStreamIndex << " " << separator << std::endl;
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

int main( int argc, char** argv )
{
	if( argc != 2 )
	{
		std::cout << "avmeta require a media filename" << std::endl;
		return( -1 );
	}

	// a simply metadata getter
	displayMetadatas( argv[1] );
}
