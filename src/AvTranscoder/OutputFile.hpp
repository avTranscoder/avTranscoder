#ifndef _AV_TRANSCODER_OUTPUT_FILE_HPP_
#define _AV_TRANSCODER_OUTPUT_FILE_HPP_

#include "DatasStructures/Image.hpp"
#include "DatasStructures/DataStreamDesc.hpp"
#include "DatasStructures/VideoDesc.hpp"

#include <string>
#include <vector>

class AVOutputFormat;
class AVFormatContext;
class AVStream;
class AVCodec;
class AVCodecContext;

namespace avtranscoder
{

class OutputFile
{
public:
	OutputFile( const std::string& file = "" );

	bool setup();

	bool addVideoStream( const VideoDesc& videoDesc );


	bool addVideoStream( const std::string& codecName );

	bool addAudioStream( );


	bool wrap( const Image& data, const size_t streamId );

	bool wrap( const DataStream& data, const size_t streamId );

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
