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
private:
	OutputFile( const OutputFile& outputFile );
	OutputFile& operator=( const OutputFile& outputFile );

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

	/**
	 * @brief Open ressource and write header.
         */
	bool beginWrap();

	IOutputStream::EWrappingStatus wrap( const CodedData& data, const size_t streamId );

	/**
	 * @brief Close ressource and write trailer.
         */
	bool endWrap();

	/**
	 * @brief Add metadata to the output file.
	 * @note Depending on the format, you are not sure to find your metadata after the transcode.
	 */
	void addMetadata( const PropertyVector& data );
	void addMetadata( const std::string& key, const std::string& value );
	
	IOutputStream& getStream( const size_t streamId );

	std::string getFilename() const { return _filename; }

	/**
	 * @brief A comma separated list of short names for the format, or empty if unknown.
	 */
	std::string getFormatName() const;

	/**
	 * @brief Descriptive name for the format, meant to be more human-readable than name, or empty if unknown.
	 */
	std::string getFormatLongName() const;

	/**
	 * @brief Comma-separated list of mime types, or empty if unknown.
	 */
	std::string getFormatMimeType() const;

	FormatContext& getFormatContext() { return _formatContext; }

	/**
	 * @brief Set the format of the output file
	 * @param profile: the profile of the output format
	 */
	void setupWrapping( const ProfileLoader::Profile& profile );

private:
	FormatContext _formatContext;
	std::vector<OutputStream*> _outputStreams;  ///< Has ownership
	std::vector<size_t> _frameCount;  ///< Number of wrapped frames

	std::string _filename;  ///< Output filename
	double _previousProcessedStreamDuration;  ///< To manage process streams order
};

}

#endif
