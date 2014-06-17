#ifndef _AV_TRANSCODER_OUTPUT_FILE_HPP_
#define _AV_TRANSCODER_OUTPUT_FILE_HPP_

#include "DatasStructures/Image.hpp"
#include "DatasStructures/DataStreamDesc.hpp"
#include "DatasStructures/VideoDesc.hpp"
#include "DatasStructures/AudioDesc.hpp"

#include <string>
#include <vector>

class AVOutputFormat;
class AVFormatContext;
class AVStream;
class AVCodec;
class AVCodecContext;

namespace avtranscoder
{

/**
 * Outputfile is a simple C++ API to wrap and write medias.
 * the default implentation use avformat wrapper frome the LibAV/FFMpeg 
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
	virtual void addVideoStream( const VideoDesc& videoDesc );

	/**
	 * @brief Add an audio output stream using the description.
	 * @note call setup() before adding any stream
	 * @param audioDesc description of output stream
	**/
	virtual void addAudioStream( const AudioDesc& audioDesc );

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

private:
	AVOutputFormat*  outputFormat;
	AVFormatContext* formatContext;

	AVCodec*         codec;
	AVCodecContext*  codecContext;
	AVStream*        stream;

	std::string filename;

	size_t packetCount;
};

}

#endif
