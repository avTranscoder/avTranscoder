#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_FILE_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_FILE_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/mediaProperty/VideoProperties.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>
#include <AvTranscoder/mediaProperty/DataProperties.hpp>
#include <AvTranscoder/mediaProperty/SubtitleProperties.hpp>
#include <AvTranscoder/mediaProperty/AttachementProperties.hpp>
#include <AvTranscoder/mediaProperty/UnknownProperties.hpp>

extern "C" {
#include <libavformat/avformat.h>
}

#include <string>
#include <vector>

namespace avtranscoder
{

class AvExport FileProperties
{
public:
	FileProperties( const AVFormatContext* formatContext );

	std::string getFilename() const;
	std::string getFormatName() const;
	std::string getFormatLongName() const;

	size_t getProgramsCount() const;
	double getStartTime() const;
	double getDuration() const;
	size_t getBitRate() const;
	size_t getPacketSize() const;

	PropertiesMap& getMetadatas() { return _metadatas; }

	size_t getNbStreams() const;
	size_t getNbVideoStreams() const { return _videoStreams.size(); }
	size_t getNbAudioStreams() const { return _audioStreams.size(); }
	size_t getNbDataStreams() const { return _dataStreams.size(); }
	size_t getNbSubtitleStreams() const { return _subtitleStreams.size(); }
	size_t getNbAttachementStreams() const { return _attachementStreams.size(); }
	size_t getNbUnknownStreams() const { return _unknownStreams.size(); }

	std::vector< avtranscoder::VideoProperties >& getVideoProperties() { return  _videoStreams; }
	const std::vector< avtranscoder::VideoProperties >& getVideoProperties() const  { return  _videoStreams; }
	std::vector< avtranscoder::AudioProperties >& getAudioProperties() { return  _audioStreams; }
	const std::vector< avtranscoder::AudioProperties >& getAudioProperties() const  { return  _audioStreams; }
	std::vector< avtranscoder::DataProperties >& getDataProperties() { return  _dataStreams; }
	const std::vector< avtranscoder::DataProperties >& getDataProperties() const  { return  _dataStreams; }
	std::vector< avtranscoder::SubtitleProperties >& getSubtitleProperties() { return  _subtitleStreams; }
	const std::vector< avtranscoder::SubtitleProperties >& getSubtitleProperties() const  { return  _subtitleStreams; }
	std::vector< avtranscoder::AttachementProperties >& getAttachementProperties() { return  _attachementStreams; }
	const std::vector< avtranscoder::AttachementProperties >& getAttachementProperties() const  { return  _attachementStreams; }
	std::vector< avtranscoder::UnknownProperties >& getUnknownPropertiesProperties() { return  _unknownStreams; }
	const std::vector< avtranscoder::UnknownProperties >& getUnknownPropertiesProperties() const  { return  _unknownStreams; }

#ifndef SWIG
	const AVFormatContext& getAVFormatContext() { return *_formatContext; }
#endif

	PropertiesMap getPropertiesAsMap() const;  ///< Return all file properties as a map (name of property: value)

private:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)

	std::vector< VideoProperties > _videoStreams;  ///< Array of properties per video stream
	std::vector< AudioProperties >  _audioStreams;  ///< Array of properties per audio stream
	std::vector< DataProperties > _dataStreams;  ///< Array of properties per data stream
	std::vector< SubtitleProperties > _subtitleStreams;  ///< Array of properties per subtitle stream
	std::vector< AttachementProperties > _attachementStreams;  ///< Array of properties per attachement stream
	std::vector< UnknownProperties > _unknownStreams;  ///< Array of properties per unknown stream

	PropertiesMap _metadatas;
};

}

#endif
