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

class AvExport OutputFile
{
public:
	OutputFile( const std::string& file = "" );

	bool setup();

	void addVideoStream( const VideoDesc& videoDesc );

	bool addAudioStream( );

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
