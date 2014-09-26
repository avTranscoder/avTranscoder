#ifndef _AV_TRANSCODER_FILE_OUTPUT_FILE_HPP_
#define _AV_TRANSCODER_FILE_OUTPUT_FILE_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/mediaProperty/mediaProperty.hpp>

#include <AvTranscoder/codec/DataStream.hpp>
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/codec/DataCodec.hpp>

#include <AvTranscoder/codedStream/AvOutputStream.hpp>

#include <AvTranscoder/Profile.hpp>

#include <string>
#include <vector>

class AVOutputFormat;
class AVFormatContext;
class AVCodec;
class AVCodecContext;
class AVStream;

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
	virtual bool wrap( const DataStream& data, const size_t streamId );

	/**
	 * @brief Finalize the end of the wrapping
	 * @note this method write the footer of file if necessary
	**/
	virtual bool endWrap( );
	
	/**
	 * @brief Set the format of the output file
	 * @param desc: the profile of the output format
	 */
	virtual void setProfile( const Profile::ProfileDesc& desc );
	
	/**
	 * @brief Add metadata to the output file.
	 * @note Depending on the format, you are not sure to find your metadata after the transcode.
	 */
	virtual void addMetadata( const MetadatasMap& dataMap );
	virtual void addMetadata( const std::string& key, const std::string& value );

	virtual void setVerbose( bool verbose = false ){ _verbose = verbose; }

private:
	std::vector<AvOutputStream*> _outputStreams;
	AVOutputFormat*  _outputFormat;
	AVFormatContext* _formatContext;

	AVCodec*         _codec;
	AVCodecContext*  _codecContext;
	AVStream*        _stream;

	std::vector<size_t> _frameCount;

	std::string      _filename;

	size_t           _packetCount;

	bool             _verbose;
};

}

#endif
