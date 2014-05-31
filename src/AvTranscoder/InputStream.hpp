#ifndef _AV_TRANSCODER_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_HPP_

#include "DatasStructures/DataStreamDesc.hpp"
#include "DatasStructures/AudioDesc.hpp"
#include "DatasStructures/VideoDesc.hpp"

namespace avtranscoder
{

class InputStream
{
public:
	InputStream( )
	{}

	virtual ~InputStream(){};

	virtual size_t getStreamIndex() const = 0;

	virtual bool readNextPacket( DataStream& data ) = 0;

	// Stream propeerties
	virtual VideoDesc getVideoDesc() const = 0;
	virtual AudioDesc getAudioDesc() const = 0;

	virtual double getDuration() const = 0;
	virtual double getPacketDuration() const = 0;

	virtual void setBufferred( const bool bufferized ) = 0;

	virtual void clearBuffering() = 0;

private:
};

}

#endif