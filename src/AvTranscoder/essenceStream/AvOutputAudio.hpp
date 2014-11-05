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

	/**
	 * @param[out] codedFrame
	 */
	bool encodeFrame( const Frame& sourceFrame, Frame& codedFrame );
	
	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( Frame& codedFrame );
	
	void setProfile( const ProfileLoader::Profile& profile, const AudioFrameDesc& frameDesc );

	ICodec& getCodec() { return _codec; }
	AudioCodec& getAudioCodec() { return _codec; }

private:
	AudioCodec _codec;
};

}

#endif