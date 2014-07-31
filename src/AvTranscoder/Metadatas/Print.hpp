#ifndef _AV_TRANSCODER_PRINT_HPP_
#define _AV_TRANSCODER_PRINT_HPP_

#include <AvTranscoder/File/InputFile.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>

namespace avtranscoder
{

static const size_t keyWidth = 32;
static const std::string separator = "====================";

std::ostream& operator<<( std::ostream& flux, const Properties& properties )
{
	flux << std::left;
	flux << separator << " Wrapper " << separator << std::endl;
	flux << std::setw( keyWidth ) << "filename"                      << ": " << properties.filename << std::endl;
	flux << std::setw( keyWidth ) << "format name"                   << ": " << properties.formatName << std::endl;
	flux << std::setw( keyWidth ) << "format long name"              << ": " << properties.formatLongName << std::endl;
	flux << std::setw( keyWidth ) << "start time"                    << ": " << properties.startTime << std::endl;
	flux << std::setw( keyWidth ) << "duration"                      << ": " << properties.duration << std::endl;
	flux << std::setw( keyWidth ) << "bitrate"                       << ": " << properties.bitRate << std::endl;
	flux << std::setw( keyWidth ) << "number of streams"             << ": " << properties.streamsCount << std::endl;
	flux << std::setw( keyWidth ) << "number of programs"            << ": " << properties.programsCount << std::endl;
	flux << std::setw( keyWidth ) << "number of video streams"       << ": " << properties.videoStreams.size() << std::endl;
	flux << std::setw( keyWidth ) << "number of audio streams"       << ": " << properties.audioStreams.size() << std::endl;
	flux << std::setw( keyWidth ) << "number of data streams"        << ": " << properties.dataStreams.size() << std::endl;
	flux << std::setw( keyWidth ) << "number of subtitle streams"    << ": " << properties.subtitleStreams.size() << std::endl;
	flux << std::setw( keyWidth ) << "number of attachement streams" << ": " << properties.attachementStreams.size() << std::endl;
	flux << std::setw( keyWidth ) << "number of unknown streams"     << ": " << properties.unknownStreams.size() << std::endl;
	
	flux << separator << " Metadatas " << separator << std::endl;
	for( size_t metadataIndex = 0; metadataIndex < properties.metadatas.size(); ++metadataIndex )
	{
		flux << std::setw( keyWidth ) << properties.metadatas.at( metadataIndex ).first << ": " << properties.metadatas.at( metadataIndex ).second << std::endl;
	}
	
	return flux;
}

std::ostream& operator<<( std::ostream& flux, const VideoProperties& videoProperties )
{
	flux << std::left;
	flux << separator << " Video stream " << separator << std::endl;
	flux << std::setw( keyWidth ) << "codec name"       << ": " << videoProperties.codecName << std::endl;
	flux << std::setw( keyWidth ) << "codec long name"  << ": " << videoProperties.codecLongName << std::endl;
	flux << std::setw( keyWidth ) << "codec id"         << ": " << videoProperties.codecId << std::endl;
	flux << std::setw( keyWidth ) << "stream id"        << ": " << videoProperties.streamId << std::endl;
	flux << std::setw( keyWidth ) << "profile name"     << ": " << videoProperties.profileName << std::endl;
	flux << std::setw( keyWidth ) << "profile"          << ": " << videoProperties.profile << std::endl;
	flux << std::setw( keyWidth ) << "level"            << ": " << videoProperties.level << std::endl;
	flux << std::setw( keyWidth ) << "width"            << ": " << videoProperties.width << std::endl;
	flux << std::setw( keyWidth ) << "height"           << ": " << videoProperties.height << std::endl;
	flux << std::setw( keyWidth ) << "dtgActiveFormat"  << ": " << videoProperties.dtgActiveFormat << std::endl;


	flux << std::setw( keyWidth ) << "start timecode"   << ": " << videoProperties.startTimecode << std::endl;
	flux << std::setw( keyWidth ) << "timeBase"         << ": " << videoProperties.timeBase.num << "/" <<
																		videoProperties.timeBase.den << std::endl;
	flux << std::setw( keyWidth ) << "fps"              << ": " << videoProperties.fps << std::endl;
	flux << std::setw( keyWidth ) << "ticksPerFrame"    << ": " << videoProperties.ticksPerFrame << std::endl;

	flux << std::setw( keyWidth ) << "pixel aspect ratio" << ": " << videoProperties.sar.num << "/" <<
																		  videoProperties.sar.den << std::endl;
	flux << std::setw( keyWidth ) << "display aspect ratio" << ": " << videoProperties.dar.num << "/" <<
																			videoProperties.dar.den << std::endl;
	flux << std::setw( keyWidth ) << "pixel type"       << ": " << videoProperties.pixelName << std::endl;
	flux << std::setw( keyWidth ) << "bit wise acked"   << ": " << ( videoProperties.bitWisePacked ? "True" : "False" ) << std::endl;
	flux << std::setw( keyWidth ) << "rgb pixel"        << ": " << ( videoProperties.rgbPixelData ? "True" : "False" ) << std::endl;
	flux << std::setw( keyWidth ) << "as alpha"         << ": " << ( videoProperties.asAlpha ? "True" : "False" ) << std::endl;
	flux << std::setw( keyWidth ) << "endianess"        << ": " << videoProperties.endianess << std::endl;

	flux << std::setw( keyWidth ) << "bit rate"         << ": " << videoProperties.bitRate << std::endl;
	flux << std::setw( keyWidth ) << "max bit rate"     << ": " << videoProperties.maxBitRate << std::endl;
	flux << std::setw( keyWidth ) << "min bit rate"     << ": " << videoProperties.minBitRate << std::endl;

	flux << std::setw( keyWidth ) << "color transfert"  << ": " << videoProperties.colorTransfert << std::endl;
	flux << std::setw( keyWidth ) << "colorspace"       << ": " << videoProperties.colorspace << std::endl;
	flux << std::setw( keyWidth ) << "color range"      << ": " << videoProperties.colorRange << std::endl;
	flux << std::setw( keyWidth ) << "color primaries"  << ": " << videoProperties.colorPrimaries << std::endl;
	flux << std::setw( keyWidth ) << "indexed colors"   << ": " << ( videoProperties.indexedColors ? "True" : "False" ) << std::endl;
	flux << std::setw( keyWidth ) << "pseudo paletted"  << ": " << ( videoProperties.pseudoPaletted ? "True" : "False" ) << std::endl;
	flux << std::setw( keyWidth ) << "components count" << ": " << videoProperties.componentsCount << std::endl;
	flux << std::setw( keyWidth ) << "chroma width"     << ": " << videoProperties.chromaWidth << std::endl;
	flux << std::setw( keyWidth ) << "chroma height"    << ": " << videoProperties.chromaHeight << std::endl;
	flux << std::setw( keyWidth ) << "chroma sample location" << ": " << videoProperties.chromaSampleLocation << std::endl;
	flux << std::setw( keyWidth ) << "interlaced "      << ": " << ( videoProperties.isInterlaced ? "True" : "False" ) << std::endl;
	flux << std::setw( keyWidth ) << "top field first"  << ": " << ( videoProperties.topFieldFirst ? "True" : "False" ) << std::endl;
	flux << std::setw( keyWidth ) << "field order"      << ": " << videoProperties.fieldOrder << std::endl;
	flux << std::setw( keyWidth ) << "gop size"         << ": " << videoProperties.gopSize << std::endl;
	flux << std::setw( keyWidth ) << "has B frames"     << ": " << ( videoProperties.hasBFrames ? "True" : "False" ) << std::endl;

	flux << std::setw( keyWidth ) << "gop" << ": ";
	for( size_t frameIndex = 0; frameIndex < videoProperties.gopStructure.size(); ++frameIndex )
	{
		flux << videoProperties.gopStructure.at( frameIndex ).first;
		flux << ( videoProperties.gopStructure.at( frameIndex ).second ? "*" : " " );
	}
	flux << std::endl;
	flux << std::setw( keyWidth ) << "references frames" << ": " << videoProperties.referencesFrames << std::endl;
	
	return flux;
}

std::ostream& operator<<( std::ostream& flux, const AudioProperties& audioProperties )
{
	flux << std::left;
	flux << separator << " Audio stream " << separator << std::endl;
	flux << std::setw( keyWidth ) << "codec name" << ": " << audioProperties.codecName << std::endl;
	flux << std::setw( keyWidth ) << "codec long name" << ": " << audioProperties.codecLongName << std::endl;
	flux << std::setw( keyWidth ) << "sample format" << ": " << audioProperties.sampleFormat << std::endl;
	flux << std::setw( keyWidth ) << "codec id" << ": " << audioProperties.codecId << std::endl;
	flux << std::setw( keyWidth ) << "stream id" << ": " << audioProperties.streamId << std::endl;
	flux << std::setw( keyWidth ) << "sample rate" << ": " << audioProperties.sampleRate << std::endl;
	flux << std::setw( keyWidth ) << "channels" << ": " << audioProperties.channels << std::endl;
	flux << std::setw( keyWidth ) << "channel layout" << ": " << audioProperties.channelLayout << std::endl;
	flux << std::setw( keyWidth ) << "channel name" << ": " << audioProperties.channelName << std::endl;
	flux << std::setw( keyWidth ) << "channel description" << ": " << audioProperties.channelDescription << std::endl;
	flux << std::setw( keyWidth ) << "bit rate" << ": " << audioProperties.bit_rate << std::endl;
	
	return flux;
}

std::ostream& operator<<( std::ostream& flux, const DataProperties& dataProperties )
{
	flux << separator << " Data stream " << separator << std::endl;
	flux << std::setw( keyWidth ) << "stream id" << ": " << dataProperties.streamId << std::endl;
	
	return flux;
}

std::ostream& operator<<( std::ostream& flux, const InputFile& input )
{
	// wrapper
	flux << input.getProperties() << std::endl;

	// video streams
	for( size_t videoStreamIndex = 0; videoStreamIndex < input.getProperties().videoStreams.size(); ++videoStreamIndex )
	{
		flux << input.getProperties().videoStreams.at( videoStreamIndex ) << std::endl;
	}
	
	// audio streams
	for( size_t audioStreamIndex = 0; audioStreamIndex < input.getProperties().audioStreams.size(); ++audioStreamIndex )
	{
		flux << input.getProperties().audioStreams.at( audioStreamIndex ) << std::endl;
	}
	
	// data streams
	for( size_t dataStreamIndex = 0; dataStreamIndex < input.getProperties().dataStreams.size(); ++dataStreamIndex )
	{
		flux << input.getProperties().dataStreams.at( dataStreamIndex ) << std::endl;
	}
	
	return flux;
}

}

#endif