#ifndef _AV_TRANSCODER_OUTPUT_STREAM_HPP_
#define _AV_TRANSCODER_OUTPUT_STREAM_HPP_

#include "DatasStructures/DataStreamDesc.hpp"
#include "DatasStructures/AudioDesc.hpp"
#include "DatasStructures/VideoDesc.hpp"

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