#ifndef _AV_TRANSCODER_ESSENCE_STREAM_DUMMY_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_DUMMY_AUDIO_HPP_

#include "InputEssence.hpp"

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/CodedStructures/AudioDesc.hpp>
#include <AvTranscoder/EssenceStructures/Frame.hpp>

namespace avtranscoder
{

class AvExport DummyAudio : public InputEssence
{
public:
	DummyAudio( );

	~DummyAudio( );

	// Stream properties
	void setAudioDesc( const AudioDesc& audioDesc );

	AudioDesc getAudioDesc() const;
	
	void setup() {}
	
	void setFrame( Frame& inputFrame );

	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	AudioDesc      _audioDesc;
	AudioFrameDesc _frameDesc;
	
	Frame*    _inputFrame;
};

}

#endif