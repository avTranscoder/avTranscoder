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
	AvInputStream*     _inputStream;
	AVCodec*           _codec;
	AVCodecContext*    _codecContext;
	AVFrame*           _frame;

	int                _selectedStream;
};

}

#endif
