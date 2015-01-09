#ifndef _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_AUDIO_HPP_

#include "IDecoder.hpp"
#include <AvTranscoder/codec/AudioCodec.hpp>

namespace avtranscoder
{

class AvExport GeneratorAudio : public IDecoder
{
public:
	GeneratorAudio( );

	AudioFrameDesc& getAudioFrameDesc() { return _frameDesc; }
	void setAudioFrameDesc( const AudioFrameDesc& frameDesc );
	
	void setup() {}
	
	void setFrame( Frame& inputFrame );

	bool decodeNextFrame( Frame& frameBuffer );
	bool decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	Frame* _inputFrame;  ///< Has link (no ownership)
	AudioFrameDesc _frameDesc;
};

}

#endif