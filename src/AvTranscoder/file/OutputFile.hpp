#ifndef _AV_TRANSCODER_FILE_OUTPUT_FILE_HPP_
#define _AV_TRANSCODER_FILE_OUTPUT_FILE_HPP_

#include <AvTranscoder/file/IOutputFile.hpp>

#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>

#include <vector>

namespace avtranscoder
{

/**
 * @brief Outputfile is the default implentation of wrapper which uses LibAV/FFMpeg.
 **/
class AvExport OutputFile : public IOutputFile
{
public:
	/**
	 * @brief Open an output media file
	 * @param filename resource to access
	**/
	OutputFile( const std::string& filename = "" );

	~OutputFile();

	IOutputStream& addVideoStream( const VideoCodec& videoDesc );
	IOutputStream& addAudioStream( const AudioCodec& audioDesc );
	IOutputStream& addDataStream( const DataCodec& dataDesc );

	bool beginWrap();
	IOutputStream::EWrappingStatus wrap( const CodedData& data, const size_t streamId );
	bool endWrap();

	/**
	 * @brief Add metadata to the output file.
	 * @note Depending on the format, you are not sure to find your metadata after the transcode.
	 */
	void addMetadata( const PropertiesMap& dataMap );
	void addMetadata( const std::string& key, const std::string& value );
	
	IOutputStream& getStream( const size_t streamId );
	FormatContext& getFormatContext() { return _formatContext; }

	/**
	 * @brief Set the format of the output file
	 * @param profile: the profile of the output format
	 */
	void setProfile( const ProfileLoader::Profile& profile );

private:
	FormatContext _formatContext;
	std::vector<OutputStream*> _outputStreams;  ///< Has ownership
	std::vector<size_t> _frameCount;  ///< Number of wrapped frames

	std::string _filename;  ///< Output filename
	double _previousProcessedStreamDuration;  ///< To manage process streams order
};

}

#endif
