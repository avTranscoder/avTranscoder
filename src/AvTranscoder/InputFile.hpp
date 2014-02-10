#ifndef _AV_TRANSCODER_INPUT_FILE_HPP_
#define _AV_TRANSCODER_INPUT_FILE_HPP_

#include "DatasStructures/Image.hpp"
#include "DatasStructures/VideoDesc.hpp"

#include <string>
#include <vector>

class AVInputFormat;
class AVFormatContext;
class AVStream;
class AVCodec;
class AVCodecContext;

namespace avtranscoder
{

class InputFile
{
public:
	InputFile( const std::string& file = "" );

	bool setup();

	VideoDesc getVideoDesc( size_t videoStreamId );
	bool getAudioStream( );


	bool unwrap( const Image& data, const size_t streamId );

private:
	AVInputFormat*   m_inputFormat;
	AVFormatContext* m_formatContext;

	AVCodec*         m_codec;
	AVCodecContext*  m_codecContext;
	AVStream*        m_stream;

	std::string      m_filename;

	size_t           m_packetCount;
};

}

#endif
