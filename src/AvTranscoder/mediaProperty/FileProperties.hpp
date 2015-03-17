#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_FILE_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_FILE_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>
#include <AvTranscoder/mediaProperty/VideoProperties.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>
#include <AvTranscoder/mediaProperty/DataProperties.hpp>
#include <AvTranscoder/mediaProperty/SubtitleProperties.hpp>
#include <AvTranscoder/mediaProperty/AttachementProperties.hpp>
#include <AvTranscoder/mediaProperty/UnknownProperties.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

class AvExport FileProperties
{
public:
	FileProperties( const FormatContext& formatContext );

	std::string getFilename() const;
	std::string getFormatName() const;  ///< A comma separated list of short names for the format
	std::string getFormatLongName() const;

	size_t getProgramsCount() const;
	double getStartTime() const;
	double getDuration() const;  ///< in seconds
	size_t getBitRate() const;  ///< total stream bitrate in bit/s, 0 if not available (result of a computation by ffmpeg)
	size_t getPacketSize() const;

	PropertiesMap& getMetadatas() { return _metadatas; }

	size_t getNbStreams() const;
	size_t getNbVideoStreams() const { return _videoStreams.size(); }
	size_t getNbAudioStreams() const { return _audioStreams.size(); }
	size_t getNbDataStreams() const { return _dataStreams.size(); }
	size_t getNbSubtitleStreams() const { return _subtitleStreams.size(); }
	size_t getNbAttachementStreams() const { return _attachementStreams.size(); }
	size_t getNbUnknownStreams() const { return _unknownStreams.size(); }

	//@{
	// @brief Get the properties with the indicated stream index
	avtranscoder::VideoProperties& getVideoPropertiesWithStreamIndex( const size_t streamIndex );
	avtranscoder::AudioProperties& getAudioPropertiesWithStreamIndex( const size_t streamIndex );
	//@}

	//@{
	// @brief Get the list of properties for a given type (video, audio...)
	std::vector< avtranscoder::VideoProperties >& getVideoProperties() { return  _videoStreams; }
	std::vector< avtranscoder::AudioProperties >& getAudioProperties() { return  _audioStreams; }
	std::vector< avtranscoder::DataProperties >& getDataProperties() { return  _dataStreams; }
	std::vector< avtranscoder::SubtitleProperties >& getSubtitleProperties() { return  _subtitleStreams; }
	std::vector< avtranscoder::AttachementProperties >& getAttachementProperties() { return  _attachementStreams; }
	std::vector< avtranscoder::UnknownProperties >& getUnknownPropertiesProperties() { return  _unknownStreams; }
	//@}

#ifndef SWIG
	const AVFormatContext& getAVFormatContext() { return *_formatContext; }

	const avtranscoder::VideoProperties& getVideoPropertiesWithStreamIndex( const size_t streamIndex ) const;
	const avtranscoder::AudioProperties& getAudioPropertiesWithStreamIndex( const size_t streamIndex ) const;

	const std::vector< avtranscoder::VideoProperties >& getVideoProperties() const  { return  _videoStreams; }
	const std::vector< avtranscoder::AudioProperties >& getAudioProperties() const  { return  _audioStreams; }
	const std::vector< avtranscoder::DataProperties >& getDataProperties() const  { return  _dataStreams; }
	const std::vector< avtranscoder::SubtitleProperties >& getSubtitleProperties() const  { return  _subtitleStreams; }
	const std::vector< avtranscoder::AttachementProperties >& getAttachementProperties() const  { return  _attachementStreams; }
	const std::vector< avtranscoder::UnknownProperties >& getUnknownPropertiesProperties() const  { return  _unknownStreams; }
#endif

	PropertiesMap getPropertiesAsMap() const;  ///< Return all file properties as a map (name of property: value)

	void clearStreamProperties();  ///< Clear all array of stream properties

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
