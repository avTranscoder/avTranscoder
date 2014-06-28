#ifndef _AV_TRANSCODER_CODED_STREAM_OUTPUT_STREAM_HPP_
#define _AV_TRANSCODER_CODED_STREAM_OUTPUT_STREAM_HPP_

#include <AvTranscoder/DatasStructures/DataStreamDesc.hpp>
#include <AvTranscoder/DatasStructures/AudioDesc.hpp>
#include <AvTranscoder/DatasStructures/VideoDesc.hpp>

namespace avtranscoder
{

class OutputStream
{
public:
	OutputStream( ) {};

	virtual ~OutputStream() {};

	virtual size_t getStreamIndex() const = 0;

	virtual bool wrap( DataStream& data ) = 0;

	// Stream properties
	virtual VideoDesc getVideoDesc() const = 0;
	virtual AudioDesc getAudioDesc() const = 0;	
};

}

#endif