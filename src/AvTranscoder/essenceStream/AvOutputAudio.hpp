#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_AUDIO_HPP_

#include "IOutputEssence.hpp"
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/ProfileLoader.hpp>

namespace avtranscoder
{

class AvExport AvOutputAudio : public IOutputEssence
{
public:
	AvOutputAudio();
	
	void setup();

	bool encodeFrame( const Frame& sourceFrame, Frame& codedFrame );
	bool encodeFrame( Frame& codedFrame );  ///<  Encode delayed encoded frames

	ICodec& getCodec() { return _codec; }
	AudioCodec& getAudioCodec() { return _codec; }

	void setProfile( const ProfileLoader::Profile& profile, const AudioFrameDesc& frameDesc );
	void setVerbose( bool verbose = true ) { _verbose = verbose; }

private:
	AudioCodec _codec;

	bool _verbose;
};

}

#endif