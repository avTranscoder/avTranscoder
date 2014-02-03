#ifndef _AV_TRANSCODER_MEDIA_HPP_
#define _AV_TRANSCODER_MEDIA_HPP_

#include <string>
#include <vector>

class AVFormatContext;

namespace avtranscoder
{


struct Ratio {
	size_t num;
	size_t den;
};

struct Channel {
	size_t id;
	size_t chromaHeight;
	size_t bitStep;
};

struct VideoProperties {
	std::string codecName;
	std::string codecLongName;
	std::string profileName;
	std::string colorTransfert;
	std::string colorspace;
	std::string colorRange;
	std::string colorPrimaries;
	std::string chromaSampleLocation;
	std::string fieldOrder;
	
	std::string pixelName;
	std::string endianess;
	
	Ratio       timeBase;
	Ratio       sar; // sample/pixel aspect ratio
	Ratio       dar; // display aspect ratio
	
	size_t      streamId;
	size_t      codecId;
	size_t      bitRate;
	size_t      ticksPerFrame;
	size_t      width;
	size_t      height;
	size_t      gopSize;
	size_t      dtgActiveFormat;
	size_t      referencesFrames;
	int         profile;
	int         level;
	size_t      componentsCount;
	size_t      chromaWidth;
	size_t      chromaHeight;
	
	double      fps;
	
	bool        hasBFrames;
	bool        indexedColors;
	bool        bitWisePacked;
	bool        hardwareAcceleration;
	bool        notFirstPlane;
	bool        rgbPixelData;
	bool        pseudoPaletted;
	bool        asAlpha;
	bool        isInterlaced;
	
	// ( frame type / is key frame )
	std::vector< std::pair< char, bool > > gopStructure;
	std::vector<Channel> channels;
};

struct AudioProperties {
	std::string codecName;
	std::string codecLongName;
	std::string sampleFormat;
	size_t      streamId;
	size_t      codecId;
	size_t      sampleRate;
	size_t      channels;
	size_t      bit_rate;
};

struct Properties {
	std::string filename;
	std::string formatName;
	std::string formatLongName;
	size_t      streamsCount;
	size_t      programsCount;
	double      startTime;
	double      duration;
	size_t      bitRate;
	size_t      packetSize;
	std::vector< VideoProperties > videoStreams;
	std::vector< AudioProperties > audioStreams;
	std::vector< std::pair< std::string, std::string > > metadatas; // ( key, value )
};

class Media
{
public:
	Media( const std::string& inputFile = "" );

	// return true if analyse was done with success
	bool analyse();

	const Properties& getProperties() const { return properties; }

private:
	std::string filename;
	AVFormatContext* formatContext;
	Properties properties;
};

}

#endif