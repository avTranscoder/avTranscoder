#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_AUDIO_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_AUDIO_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <string>

class AVFormatContext;

namespace avtranscoder
{

class AvExport AudioProperties
{
public:
	AudioProperties( const AVFormatContext* formatContext, const size_t index );

	std::string getCodecName() const;
	std::string getCodecLongName() const;
	std::string getSampleFormatName() const;
	std::string getSampleFormatLongName() const;
	std::string getChannelLayout() const;
	std::string getChannelName() const;
	std::string getChannelDescription() const;
	size_t getStreamId() const { return _streamId; }
	size_t getCodecId() const { return _codecContext->codec_id; }
	size_t getSampleRate() const { return _codecContext->sample_rate; }
	size_t getChannels() const { return _codecContext->channels; }
	size_t getBitRate() const { return _codecContext->bit_rate; }
	MetadatasMap& getMetadatas() { return _metadatas; }

	const AVFormatContext& getAVFormatContext() { return *_formatContext; }
	AVCodecContext& getAVCodecContext() { return *_codecContext; }

	MetadatasMap getDataMap() const;

private:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)
	AVCodecContext* _codecContext;  ///< Has link (no ownership)
	AVCodec* _codec; ///< Has link (no ownership)

	size_t _streamId;
	MetadatasMap _metadatas;
};

}

#endif
