#ifndef _AV_TRANSCODER_CODED_STREAM_I_OUTPUT_STREAM_HPP_
#define _AV_TRANSCODER_CODED_STREAM_I_OUTPUT_STREAM_HPP_

#include <AvTranscoder/codedStructures/DataStream.hpp>
#include <AvTranscoder/codedStructures/AudioDesc.hpp>
#include <AvTranscoder/codedStructures/VideoDesc.hpp>

namespace avtranscoder
{

class IOutputStream
{
public:
	virtual ~IOutputStream() {};

	virtual size_t getStreamIndex() const = 0;

	virtual bool wrap( DataStream& data ) = 0;

	// Stream properties
	virtual VideoDesc getVideoDesc() const = 0;
	virtual AudioDesc getAudioDesc() const = 0;
};

}

#endif