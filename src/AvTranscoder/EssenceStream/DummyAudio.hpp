#ifndef _AV_TRANSCODER_ESSENCE_STREAM_DUMMY_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_DUMMY_AUDIO_HPP_

#include "InputEssence.hpp"

#include <AvTranscoder/DatasStructures/AudioDesc.hpp>
#include <AvTranscoder/DatasStructures/Frame.hpp>

namespace avtranscoder
{

class AvExport DummyAudio : public InputEssence
{
public:
	DummyAudio( );

	~DummyAudio( );

	// Stream properties
	void setAudioDesc( AudioDesc& audioDesc );

	AudioDesc getAudioDesc() const;
	
	void setup() {}

	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( std::vector<Frame>& frameBuffer );

private:
	AudioDesc _audioDesc;
	AudioFrameDesc _frameDesc;
};

}

#endif