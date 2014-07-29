#ifndef _AV_TRANSCODER_FILE_OUTPUT_FILE_HPP_
#define _AV_TRANSCODER_FILE_OUTPUT_FILE_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/CodedStructures/DataStream.hpp>
#include <AvTranscoder/CodedStructures/VideoDesc.hpp>
#include <AvTranscoder/CodedStructures/AudioDesc.hpp>

#include <AvTranscoder/CodedStream/AvOutputStream.hpp>

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
	virtual OutputStream& addVideoStream( const VideoDesc& videoDesc );

	/**
	 * @brief Add an audio output stream using the description.
	 * @note call setup() before adding any stream
	 * @param audioDesc description of output stream
	**/
	virtual OutputStream& addAudioStream( const AudioDesc& audioDesc );

	/**
	 * @brief get the output stream description.
	 * @param streamId select the output stream
	 * @return the output stream reference
	**/
	virtual OutputStream& getStream( const size_t streamId );

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

private:
	std::vector<AvOutputStream*> _outputStreams;
	AVOutputFormat*  _outputFormat;
	AVFormatContext* _formatContext;

	AVCodec*         _codec;
	AVCodecContext*  _codecContext;
	AVStream*        _stream;

	std::string      _filename;

	size_t           _packetCount;
};

}

#endif
