#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_AUDIO_HPP_

#include "IOutputEssence.hpp"

#include <AvTranscoder/codec/DataStream.hpp>
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>

#include <AvTranscoder/Profile.hpp>

namespace avtranscoder
{

class AvOutputAudio : public IOutputEssence
{
public:
	AvOutputAudio();
	
	void setup();

	/**
	 * @param[out] codedFrame
	 */
	bool encodeFrame( const Frame& sourceFrame, DataStream& codedFrame );
	
	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( DataStream& codedFrame );
	
	void setProfile( const Profile::ProfileDesc& desc, const AudioFrameDesc& frameDesc );

	AudioCodec getAudioCodec() { return _codedDesc; }
};

}

#endif