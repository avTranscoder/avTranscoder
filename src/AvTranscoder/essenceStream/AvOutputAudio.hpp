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
	AvOutputAudio( const std::string& audioCodecName );
	
	void setup();

	bool encodeFrame( const Frame& sourceFrame, Frame& codedFrame );
	bool encodeFrame( Frame& codedFrame );

	ICodec& getCodec() { return _codec; }
	AudioCodec& getAudioCodec() { return _codec; }

	void setProfile( const ProfileLoader::Profile& profile, const AudioFrameDesc& frameDesc );

private:
	AudioCodec _codec;
};

}

#endif