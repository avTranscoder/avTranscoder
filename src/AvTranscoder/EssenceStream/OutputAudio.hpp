#ifndef _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_AUDIO_HPP_

#include "OutputEssence.hpp"

#include <AvTranscoder/DatasStructures/AudioDesc.hpp>
#include <AvTranscoder/DatasStructures/DataStreamDesc.hpp>
#include <AvTranscoder/DatasStructures/AudioFrame.hpp>

#include <AvTranscoder/Profile.hpp>

namespace avtranscoder
{

class OutputAudio : public OutputEssence
{
public:
	OutputAudio();
	
	bool setup();

	/**
	 * @param[out] codedFrame
	 */
	bool encodeFrame( const Frame& sourceFrame, DataStream& codedFrame );
	
	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( DataStream& codedFrame );
	
	void setProfile( Profile::ProfileDesc& desc, const AudioFrameDesc& frameDesc );

	AudioDesc& getAudioDesc() { return _audioDesc; }

private:
	AudioDesc _audioDesc;
};

}

#endif