#ifndef _MEDIA_MANAGER_IO_CODED_STREAM_I_INPUT_STREAM_HPP_
#define _MEDIA_MANAGER_IO_CODED_STREAM_I_INPUT_STREAM_HPP_

#include <mediaCore/frame/Frame.hpp>
#include <mediaIO/codec/AudioCodec.hpp>
#include <mediaIO/codec/VideoCodec.hpp>
#include <mediaIO/codec/DataCodec.hpp>

namespace mediamanager
{
namespace mediaio
{

class IInputStream
{
public:
	virtual ~IInputStream() {};

	virtual size_t getStreamIndex() const = 0;

	virtual bool readNextPacket( mediacore::CodedData& data ) = 0;

	// Stream properties
	virtual VideoCodec& getVideoCodec() = 0;
	virtual AudioCodec& getAudioCodec() = 0;
	virtual DataCodec&  getDataCodec() = 0;
	
	virtual AVMediaType getStreamType() const = 0;

	virtual double getDuration() const = 0;
	virtual double getPacketDuration() const = 0;

	virtual void setBufferred( const bool bufferized ) = 0;

	virtual void clearBuffering() = 0;

};

}
}

#endif