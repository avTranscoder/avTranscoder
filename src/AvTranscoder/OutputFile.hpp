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

class AvExport OutputFile
{
public:
	OutputFile( const std::string& file = "" );

	virtual ~OutputFile(){};

	virtual bool setup();

	virtual void addVideoStream( const VideoDesc& videoDesc );
	virtual void addAudioStream( const AudioDesc& audioDesc );

	virtual bool beginWrap( );

	virtual bool wrap( const DataStream& data, const size_t streamId );

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
