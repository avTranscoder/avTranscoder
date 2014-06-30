#ifndef _AV_TRANSCODER_ESSENCE_STREAM_INPUT_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_INPUT_AUDIO_HPP_

#include "InputEssence.hpp"
#include <AvTranscoder/DatasStructures/AudioFrame.hpp>

class AVFormatContext;
class AVCodec;
class AVCodecContext;
class AVFrame;

namespace avtranscoder
{

class AvInputStream;

class AvExport InputAudio : public InputEssence
{
public:
	InputAudio( AvInputStream& inputStream );
	~InputAudio();

	void setup();

	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( std::vector<Frame>& frameBuffer );

private:
	bool getNextFrame();

	AvInputStream*     _inputStream;
	AVCodec*           _codec;
	AVCodecContext*    _codecContext;
	AVFrame*           _frame;

	int                _selectedStream;
};

}

#endif
