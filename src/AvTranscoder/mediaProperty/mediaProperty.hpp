#ifndef _AV_TRANSCODER_MEDIA_HPP_
#define _AV_TRANSCODER_MEDIA_HPP_

#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavutil/rational.h>
}

#include <string>
#include <vector>
#include <map>

namespace avtranscoder
{

/**
 * @brief Can get all data of Properties structures by getDataMap(), which return a MetadatasMap.
 */
typedef std::vector< std::pair<std::string, std::string> > MetadatasMap;

namespace detail
{
	/**
	 * @brief Fill metadata parameter with the given AVDictionary.
	 */
	void AvExport fillMetadataDictionnary( AVDictionary* avdictionnary, MetadatasMap& metadata );
}

struct AvExport Channel
{
	size_t id;
	size_t chromaHeight;
	size_t bitStep;
};

struct AvExport VideoProperties
{
	VideoProperties()
	{
		timeBase.num = 0;
		timeBase.den = 0;
		sar.num = 0;
		sar.den = 0;
		dar.num = 0;
		dar.den = 0;
	}
	
public:
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
	
	std::string startTimecode;

	AVRational timeBase;
	AVRational sar; // sample/pixel aspect ratio
	AVRational dar; // display aspect ratio
	
	size_t      streamId;
	size_t      codecId;
	size_t      bitRate;
	size_t      maxBitRate;
	size_t      minBitRate;
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
	bool        topFieldFirst;
	
	// ( frame type / is key frame )
	std::vector< std::pair< char, bool > > gopStructure;
	std::vector<Channel> channels;

	MetadatasMap metadatas;

public:
	MetadatasMap getDataMap() const;
};

struct AvExport AudioProperties
{
	std::string codecName;
	std::string codecLongName;
	std::string sampleFormat;
	std::string channelLayout;
	std::string channelName;
	std::string channelDescription;
	size_t      streamId;
	size_t      codecId;
	size_t      sampleRate;
	size_t      channels;
	size_t      bit_rate;

	MetadatasMap metadatas;

public:
	MetadatasMap getDataMap() const;
};

struct AvExport DataProperties
{
	size_t      streamId;
	MetadatasMap metadatas;

public:
	MetadatasMap getDataMap() const;
};

struct AvExport SubtitleProperties
{
	size_t      streamId;
	MetadatasMap metadatas;

public:
	MetadatasMap getDataMap() const;
};

struct AvExport AttachementProperties
{
	size_t      streamId;
	MetadatasMap metadatas;

public:
	MetadatasMap getDataMap() const;
};

struct AvExport UnknownProperties
{
	size_t      streamId;
	MetadatasMap metadatas;

public:
	MetadatasMap getDataMap() const;
};

struct AvExport Properties
{
	std::string filename;
	std::string formatName;
	std::string formatLongName;
	size_t      streamsCount;
	size_t      programsCount;
	double      startTime;
	double      duration;
	size_t      bitRate;
	size_t      packetSize;
	
	std::vector< VideoProperties >       videoStreams;
	std::vector< AudioProperties >       audioStreams;
	std::vector< DataProperties >        dataStreams;
	std::vector< SubtitleProperties >    subtitleStreams;
	std::vector< AttachementProperties > attachementStreams;
	std::vector< UnknownProperties >     unknownStreams;

	MetadatasMap metadatas;

public:
	MetadatasMap getDataMap() const;
};

}

#endif