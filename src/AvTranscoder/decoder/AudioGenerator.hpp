#ifndef _AV_TRANSCODER_DECODER_AUDIO_GENERATOR_HPP_
#define _AV_TRANSCODER_DECODER_AUDIO_GENERATOR_HPP_

#include "IDecoder.hpp"
#include <AvTranscoder/codec/AudioCodec.hpp>

namespace avtranscoder
{

class AvExport AudioGenerator : public IDecoder
{
public:
	AudioGenerator();
	~AudioGenerator();

	AudioFrameDesc& getAudioFrameDesc() { return _frameDesc; }
	void setAudioFrameDesc( const AudioFrameDesc& frameDesc );
	
	void setup() {}
	
	void setFrame( Frame& inputFrame );

	bool decodeNextFrame( Frame& frameBuffer );
	bool decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	Frame* _inputFrame;  ///< Has link (no ownership)
	AudioFrame* _silent;   ///< The generated silent (has ownership)
	AudioFrameDesc _frameDesc;  ///< The description of the silent (sample rate...) 
};

}

#endif