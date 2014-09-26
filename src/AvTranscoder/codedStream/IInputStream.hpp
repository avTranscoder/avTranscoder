#ifndef _AV_TRANSCODER_CODED_STREAM_I_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_CODED_STREAM_I_INPUT_STREAM_HPP_

#include <AvTranscoder/codec/DataStream.hpp>
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/codec/DataCodec.hpp>

namespace avtranscoder
{

class IInputStream
{
public:
	virtual ~IInputStream() {};

	virtual size_t getStreamIndex() const = 0;

	virtual bool readNextPacket( DataStream& data ) = 0;

	// Stream properties
	virtual VideoCodec getVideoCodec() const = 0;
	virtual AudioCodec getAudioCodec() const = 0;
	virtual DataCodec  getDataCodec()  const = 0;
	
	virtual AVMediaType getStreamType() const = 0;

	virtual double getDuration() const = 0;
	virtual double getPacketDuration() const = 0;

	virtual void setBufferred( const bool bufferized ) = 0;

	virtual void clearBuffering() = 0;

};

}

#endif