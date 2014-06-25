#ifndef _AV_TRANSCODER_OUTPUT_STREAM_AUDIO_HPP_
#define _AV_TRANSCODER_OUTPUT_STREAM_AUDIO_HPP_

#include "OutputStreamWriter.hpp"
#include "DatasStructures/AudioDesc.hpp"
#include "DatasStructures/DataStreamDesc.hpp"

#include "Profile.hpp"

namespace avtranscoder
{

class OutputStreamAudio : public OutputStreamWriter
{
public:
	OutputStreamAudio();
	
	bool setup();

	/**
	 * @param[out] codedFrame
	 */
	bool encodeFrame( const Frame& sourceFrame, DataStream& codedFrame );
	
	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( DataStream& codedFrame );
	
	void setProfile( const std::string& profile );

	AudioDesc& getAudioDesc() { return _audioDesc; }

private:
	bool checkProfileKey( Profile::ProfileDesc& profDesc, const std::string& key );

private:
	AudioDesc _audioDesc;
};

}

#endif