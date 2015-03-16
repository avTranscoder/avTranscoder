#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_AUDIO_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_AUDIO_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>

#include <string>

namespace avtranscoder
{

class AvExport AudioProperties
{
public:
	AudioProperties( const FormatContext& formatContext, const size_t index );

	std::string getCodecName() const;
	std::string getCodecLongName() const;
	std::string getSampleFormatName() const;
	std::string getSampleFormatLongName() const;
	std::string getChannelLayout() const;
	std::string getChannelName() const;
	std::string getChannelDescription() const;

	size_t getStreamIndex() const { return _streamIndex; }
	size_t getStreamId() const;
	size_t getCodecId() const;
	size_t getSampleRate() const;
	size_t getChannels() const;
	size_t getBitRate() const;
	size_t getNbSamples() const;

	size_t getTicksPerFrame() const;
	Rational getTimeBase() const;
	double getDuration() const;

	PropertiesMap& getMetadatas() { return _metadatas; }

#ifndef SWIG
	const AVFormatContext& getAVFormatContext() { return *_formatContext; }
	AVCodecContext& getAVCodecContext() { return *_codecContext; }
#endif

	PropertiesMap getPropertiesAsMap() const;  ///< Return all audio properties as a map (name of property: value)

private:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)
	AVCodecContext* _codecContext;  ///< Has link (no ownership)
	AVCodec* _codec; ///< Has link (no ownership)

	size_t _streamIndex;
	PropertiesMap _metadatas;
};

}

#endif
