#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_AUDIO_HPP_

#include "IOutputEssence.hpp"

#include <AvTranscoder/codedStructures/DataStream.hpp>
#include <AvTranscoder/codedStructures/AudioDesc.hpp>
#include <AvTranscoder/EssenceStructures/AudioFrame.hpp>

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

	AudioDesc getAudioDesc() { return _codedDesc; }
};

}

#endif