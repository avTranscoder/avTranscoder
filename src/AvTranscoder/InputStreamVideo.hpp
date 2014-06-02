#ifndef _AV_TRANSCODER_INPUT_STREAM_VIDEO_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_VIDEO_HPP_

#include "InputFile.hpp"
#include "DatasStructures/Image.hpp"

#include <vector>

class AVFormatContext;
class AVCodec;
class AVCodecContext;
class AVFrame;

namespace avtranscoder
{

class AvInputStream;

class AvExport InputStreamVideo
{
public:
	InputStreamVideo( AvInputStream& inputStream );
	~InputStreamVideo();
	
	bool readNextFrame( Image& frameBuffer );

	void flushDecoder();

private:
	AvInputStream*     m_inputStream;
	AVCodec*           m_codec;
	AVCodecContext*    m_codecContext;
	AVFrame*           m_frame;

	int                m_selectedStream;
};

}

#endif