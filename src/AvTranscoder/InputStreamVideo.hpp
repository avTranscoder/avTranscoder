#ifndef _AV_TRANSCODER_INPUT_STREAM_VIDEO_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_VIDEO_HPP_

#include "InputStreamReader.hpp"
#include "DatasStructures/Image.hpp"

#include <vector>

class AVFormatContext;
class AVCodec;
class AVCodecContext;
class AVFrame;

namespace avtranscoder
{

class AvInputStream;

class AvExport InputStreamVideo : public InputStreamReader
{
public:
	InputStreamVideo( AvInputStream& inputStream );
	~InputStreamVideo();
	
	void setup();

	bool readNextFrame( Frame& frameBuffer );

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
