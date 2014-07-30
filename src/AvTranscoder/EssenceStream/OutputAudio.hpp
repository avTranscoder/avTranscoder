#ifndef _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_AUDIO_HPP_

#include "OutputEssence.hpp"

#include <AvTranscoder/CodedStructures/DataStream.hpp>
#include <AvTranscoder/CodedStructures/AudioDesc.hpp>
#include <AvTranscoder/EssenceStructures/AudioFrame.hpp>

#include <AvTranscoder/Profile.hpp>

namespace avtranscoder
{

class OutputAudio : public OutputEssence
{
public:
	OutputAudio();
	
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

	AudioDesc& getAudioDesc() { return _audioDesc; }

private:
	AudioDesc _audioDesc;
};

}

#endif