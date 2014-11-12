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

	std::string getFilename() const { return _formatContext->filename; }
	std::string getFormatName() const { return _formatContext->iformat->name; }
	std::string getFormatLongName() const { return _formatContext->iformat->long_name; }

	size_t getProgramsCount() const { return _formatContext->nb_programs; }
	double getStartTime() const { return  1.0 * (unsigned int)_formatContext->start_time / AV_TIME_BASE; }
	double getDuration() const { return 1.0 * _formatContext->duration / AV_TIME_BASE; }
	size_t getBitRate() const { return _formatContext->bit_rate; }
	size_t getPacketSize() const { return _formatContext->packet_size; }

	MetadatasMap& getMetadatas() { return _metadatas; }

	size_t getNbStreams() const { return _formatContext->nb_streams; }
	size_t getNbVideoStreams() const { return _videoStreams.size(); }
	size_t getNbAudioStreams() const { return _audioStreams.size(); }
	size_t getNbDataStreams() const { return _dataStreams.size(); }
	size_t getNbSubtitleStreams() const { return _subtitleStreams.size(); }
	size_t getNbAttachementStreams() const { return _attachementStreams.size(); }
	size_t getNbUnknownStreams() const { return _unknownStreams.size(); }

	std::vector< VideoProperties >& getVideoProperties() { return  _videoStreams; }
	const std::vector< VideoProperties >& getVideoProperties() const  { return  _videoStreams; }
	std::vector< AudioProperties >& getAudioProperties() { return  _audioStreams; }
	const std::vector< AudioProperties >& getAudioProperties() const  { return  _audioStreams; }
	std::vector< DataProperties >& getDataProperties() { return  _dataStreams; }
	const std::vector< DataProperties >& getDataProperties() const  { return  _dataStreams; }
	std::vector< SubtitleProperties >& getSubtitleProperties() { return  _subtitleStreams; }
	const std::vector< SubtitleProperties >& getSubtitleProperties() const  { return  _subtitleStreams; }
	std::vector< AttachementProperties >& getAttachementProperties() { return  _attachementStreams; }
	const std::vector< AttachementProperties >& getAttachementProperties() const  { return  _attachementStreams; }
	std::vector< UnknownProperties >& getUnknownPropertiesProperties() { return  _unknownStreams; }
	const std::vector< UnknownProperties >& getUnknownPropertiesProperties() const  { return  _unknownStreams; }

	const AVFormatContext& getAVFormatContext() { return *_formatContext; }

	MetadatasMap getDataMap() const;

private:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)

	std::vector< VideoProperties > _videoStreams;  ///< Array of properties per video stream
	std::vector< AudioProperties >  _audioStreams;  ///< Array of properties per audio stream
	std::vector< DataProperties > _dataStreams;  ///< Array of properties per data stream
	std::vector< SubtitleProperties > _subtitleStreams;  ///< Array of properties per subtitle stream
	std::vector< AttachementProperties > _attachementStreams;  ///< Array of properties per attachement stream
	std::vector< UnknownProperties > _unknownStreams;  ///< Array of properties per unknown stream

	MetadatasMap _metadatas;
};

}

#endif
