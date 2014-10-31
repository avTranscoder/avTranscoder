#ifndef _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_AUDIO_HPP_

#include "IInputEssence.hpp"
#include <AvTranscoder/codec/AudioCodec.hpp>

namespace avtranscoder
{

class AvExport GeneratorAudio : public IInputEssence
{
public:
	GeneratorAudio( );

	void setAudioCodec( const AudioCodec& codec );

	const AudioCodec& getAudioCodec();
	
	void setup() {}
	
	void setFrame( Frame& inputFrame );

	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	const AudioCodec* _codec;  ///< Has link (no ownership)
	Frame* _inputFrame;  ///< Has link (no ownership)
	AudioFrameDesc _frameDesc;
};

}

#endif