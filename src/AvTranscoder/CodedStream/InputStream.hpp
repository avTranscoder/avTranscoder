#ifndef _AV_TRANSCODER_CODED_STREAM_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_CODED_STREAM_INPUT_STREAM_HPP_

#include <AvTranscoder/DatasStructures/DataStreamDesc.hpp>
#include <AvTranscoder/DatasStructures/AudioDesc.hpp>
#include <AvTranscoder/DatasStructures/VideoDesc.hpp>

namespace avtranscoder
{

class InputStream
{
public:
	InputStream() {}

	virtual ~InputStream() {};

	virtual size_t getStreamIndex() const = 0;

	virtual bool readNextPacket( DataStream& data ) = 0;

	// Stream properties
	virtual VideoDesc getVideoDesc() const = 0;
	virtual AudioDesc getAudioDesc() const = 0;
	
	virtual AVMediaType getStreamType() const = 0;

	virtual double getDuration() const = 0;
	virtual double getPacketDuration() const = 0;

	virtual void setBufferred( const bool bufferized ) = 0;

	virtual void clearBuffering() = 0;

};

}

#endif