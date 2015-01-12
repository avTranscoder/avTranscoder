#ifndef _AV_TRANSCODER_FILE_OUTPUT_FILE_HPP_
#define _AV_TRANSCODER_FILE_OUTPUT_FILE_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/option/FormatContext.hpp>

#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/codec/DataCodec.hpp>

#include <AvTranscoder/codedStream/AvOutputStream.hpp>
#include <AvTranscoder/ProfileLoader.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

/**
 * @brief Outputfile is a simple C++ API to wrap and write medias.\n
 * the default implentation use avformat wrapper frome the LibAV/FFMpeg.\n
 * It can be overloaded to integrate custom wrapper.
 **/
class AvExport OutputFile
{
public:
	/**
	 * @brief Open an output media file
	 * @param filename resource to access
	**/
	OutputFile( const std::string& filename = "" );

	virtual ~OutputFile(){};

	/**
	 * @brief Initialize the OutputFile, create format context to wrap essences into output file.
	 * @note call this before adding streams using addVideoStream() or addAudioStream()
	 * @exception ios_base::failure launched if unable to guess format or open output
	**/
	virtual bool setup();

	/**
	 * @brief Add an video output stream using the description.
	 * @note call setup() before adding any stream
	 * @param videoDesc description of output stream
	**/
	virtual IOutputStream& addVideoStream( const VideoCodec& videoDesc );

	/**
	 * @brief Add an audio output stream using the description.
	 * @note call setup() before adding any stream
	 * @param audioDesc description of output stream
	**/
	virtual IOutputStream& addAudioStream( const AudioCodec& audioDesc );

	/**
	 * @brief Add an data output stream using the description.
	 * @note call setup() before adding any stream
	 * @param dataDesc description of output stream
	**/
	virtual IOutputStream& addDataStream( const DataCodec& dataDesc );

	/**
	 * @brief get the output stream description.
	 * @param streamId select the output stream
	 * @return the output stream reference
	**/
	virtual IOutputStream& getStream( const size_t streamId );

	/**
	 * @brief Initialise the wrapping
	 * @note this method write the header of file if necessary
	**/
	virtual bool beginWrap( );

	/**
	 * @brief Wrap a packet of data in the output ressource
	 * @param data coded packet information for the current stream
	 * @param streamId refers to the stream in output ressource
	**/
	virtual IOutputStream::EWrappingStatus wrap( const CodedData& data, const size_t streamId );

	/**
	 * @brief Finalize the end of the wrapping
	 * @note this method write the footer of file if necessary
	**/
	virtual bool endWrap( );
	
	/**
	 * @brief Set the format of the output file
	 * @param desc: the profile of the output format
	 */
	virtual void setProfile( const ProfileLoader::Profile& profile );
	
	/**
	 * @brief Add metadata to the output file.
	 * @note Depending on the format, you are not sure to find your metadata after the transcode.
	 */
	virtual void addMetadata( const PropertiesMap& dataMap );
	virtual void addMetadata( const std::string& key, const std::string& value );

	virtual void setVerbose( bool verbose = false ){ _verbose = verbose; }
	
	virtual double getProgressDuration();

private:
	FormatContext _formatContext;
	std::vector<AvOutputStream*> _outputStreams;  ///< Has ownership
	std::vector<size_t> _frameCount;  ///< Number of wrapped frames

	std::string _filename;  ///< Output filename
	double _previousProcessedStreamDuration;  ///< To manage process streams order
	bool _verbose;
};

}

#endif
