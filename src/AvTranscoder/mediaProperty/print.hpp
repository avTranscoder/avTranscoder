#ifndef _AV_TRANSCODER_PRINT_HPP_
#define _AV_TRANSCODER_PRINT_HPP_

#include <AvTranscoder/file/InputFile.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>

namespace avtranscoder
{

static const size_t keyWidth = 32;
static const std::string separator = "====================";

std::ostream& operator<<( std::ostream& flux, const FileProperties& fileProperties )
{
	flux << std::left;
	flux << separator << " Wrapper " << separator << std::endl;

	PropertiesMap properties = fileProperties.getDataMap();
	for( PropertiesMap::iterator it = properties.begin(); it != properties.end(); ++it )
	{
		flux << std::setw( keyWidth ) << it->first << ": " << it->second << std::endl;
	}

	return flux;
}

std::ostream& operator<<( std::ostream& flux, const VideoProperties& videoProperties )
{
	flux << std::left;
	flux << separator << " Video stream " << separator << std::endl;

	PropertiesMap properties = videoProperties.getDataMap();
	for( PropertiesMap::iterator it = properties.begin(); it != properties.end(); ++it )
	{
		flux << std::setw( keyWidth ) << it->first << ": " << it->second << std::endl;
	}

	return flux;
}

std::ostream& operator<<( std::ostream& flux, const AudioProperties& audioProperties )
{
	flux << std::left;
	flux << separator << " Audio stream " << separator << std::endl;

	PropertiesMap properties = audioProperties.getDataMap();
	for( PropertiesMap::iterator it = properties.begin(); it != properties.end(); ++it )
	{
		flux << std::setw( keyWidth ) << it->first << ": " << it->second << std::endl;
	}
	
	return flux;
}

std::ostream& operator<<( std::ostream& flux, const DataProperties& dataProperties )
{
	flux << separator << " Data stream " << separator << std::endl;

	PropertiesMap properties = dataProperties.getDataMap();
	for( PropertiesMap::iterator it = properties.begin(); it != properties.end(); ++it )
	{
		flux << std::setw( keyWidth ) << it->first << ": " << it->second << std::endl;
	}
	
	return flux;
}

std::ostream& operator<<( std::ostream& flux, const SubtitleProperties& subtitleProperties )
{
	flux << separator << " Subtitle stream " << separator << std::endl;

	PropertiesMap properties = subtitleProperties.getDataMap();
	for( PropertiesMap::iterator it = properties.begin(); it != properties.end(); ++it )
	{
		flux << std::setw( keyWidth ) << it->first << ": " << it->second << std::endl;
	}

	return flux;
}

std::ostream& operator<<( std::ostream& flux, const AttachementProperties& attachementProperties )
{
	flux << separator << " Attachement stream " << separator << std::endl;

	PropertiesMap properties = attachementProperties.getDataMap();
	for( PropertiesMap::iterator it = properties.begin(); it != properties.end(); ++it )
	{
		flux << std::setw( keyWidth ) << it->first << ": " << it->second << std::endl;
	}

	return flux;
}

std::ostream& operator<<( std::ostream& flux, const UnknownProperties& unknownProperties )
{
	flux << separator << " Unknown stream " << separator << std::endl;

	PropertiesMap properties = unknownProperties.getDataMap();
	for( PropertiesMap::iterator it = properties.begin(); it != properties.end(); ++it )
	{
		flux << std::setw( keyWidth ) << it->first << ": " << it->second << std::endl;
	}

	return flux;
}

std::ostream& operator<<( std::ostream& flux, const InputFile& input )
{
	// wrapper
	flux << input.getProperties();

	// video streams
	for( size_t videoStreamIndex = 0; videoStreamIndex < input.getProperties().getNbVideoStreams(); ++videoStreamIndex )
	{
		flux << input.getProperties().getVideoProperties().at( videoStreamIndex );
	}

	// audio streams
	for( size_t audioStreamIndex = 0; audioStreamIndex < input.getProperties().getNbAudioStreams(); ++audioStreamIndex )
	{
		flux << input.getProperties().getAudioProperties().at( audioStreamIndex );
	}

	// data streams
	for( size_t dataStreamIndex = 0; dataStreamIndex < input.getProperties().getNbDataStreams(); ++dataStreamIndex )
	{
		flux << input.getProperties().getDataProperties().at( dataStreamIndex );
	}

	// subtitle streams
	for( size_t subtitleStreamIndex = 0; subtitleStreamIndex < input.getProperties().getNbSubtitleStreams(); ++subtitleStreamIndex )
	{
		flux << input.getProperties().getSubtitleProperties().at( subtitleStreamIndex );
	}

	// attachement streams
	for( size_t attachementStreamIndex = 0; attachementStreamIndex < input.getProperties().getNbAttachementStreams(); ++attachementStreamIndex )
	{
		flux << input.getProperties().getAttachementProperties().at( attachementStreamIndex );
	}

	// unknown streams
	for( size_t unknownStreamIndex = 0; unknownStreamIndex < input.getProperties().getNbUnknownStreams(); ++unknownStreamIndex )
	{
		flux << input.getProperties().getUnknownPropertiesProperties().at( unknownStreamIndex );
	}

	return flux;
}

}

#endif
