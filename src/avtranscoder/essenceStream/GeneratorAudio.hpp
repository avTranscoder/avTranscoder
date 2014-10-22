#ifndef _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_AUDIO_HPP_

#include "IInputEssence.hpp"
#include <avtranscoder/codec/AudioCodec.hpp>

namespace avtranscoder
{

class AvExport GeneratorAudio : public IInputEssence
{
public:
	GeneratorAudio( );

	~GeneratorAudio( );

	void setAudioCodec( const AudioCodec& codec );

	AudioCodec& getAudioCodec();
	
	void setup() {}
	
	void setFrame( Frame& inputFrame );

	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	AudioCodec _codec;
	AudioFrameDesc _frameDesc;
	
	Frame*         _inputFrame;
};

}

#endif