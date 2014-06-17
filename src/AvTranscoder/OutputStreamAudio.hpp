#ifndef _AV_TRANSCODER_OUTPUT_STREAM_AUDIO_HPP_
#define _AV_TRANSCODER_OUTPUT_STREAM_AUDIO_HPP_

#include "OutputStream.hpp"
#include "DatasStructures/AudioFrame.hpp"
#include "DatasStructures/AudioDesc.hpp"
#include "DatasStructures/DataStreamDesc.hpp"

namespace avtranscoder
{

class OutputStreamAudio
{
public:
	OutputStreamAudio();
	
	bool setup();

	/**
	 * @param[out] codedFrame
	 */
	bool encodeFrame( const AudioFrame& decodedFrame, DataStream& codedFrame );
	
	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( DataStream& codedFrame );
	
	AudioDesc& getAudioDesc() { return m_audioDesc; }

private:
	AudioDesc m_audioDesc;
};

}

#endif