#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_AUDIO_HPP_

#include "IInputEssence.hpp"
#include <AvTranscoder/codec/AudioCodec.hpp>

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
	bool decodeNextFrame();

	AvInputStream* _inputStream;  ///< Stream from which we read next frames
	const AudioCodec* _codec;  ///< Audio decoder. Has link (no ownership)
	AVFrame* _frame;  ///< Libav object to store decoded data
};

}

#endif
