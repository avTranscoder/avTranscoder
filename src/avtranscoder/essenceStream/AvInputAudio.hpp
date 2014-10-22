#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_AUDIO_HPP_

#include "IInputEssence.hpp"
#include <avtranscoder/codec/AudioCodec.hpp>

struct AVFrame;

namespace avtranscoder
{

class AvInputStream;

class AvExport AvInputAudio : public IInputEssence
{
public:
	AvInputAudio( AvInputStream& inputStream );
	~AvInputAudio();

	void setup();

	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	bool getNextFrame();

	AvInputStream*     _inputStream;
	AudioCodec _codec;
	AVFrame*           _frame;

	int                _selectedStream;
};

}

#endif
