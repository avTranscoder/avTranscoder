#ifndef _AV_TRANSCODER_CODED_STREAM_I_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_CODED_STREAM_I_INPUT_STREAM_HPP_

#include <AvTranscoder/mediaProperty/StreamProperties.hpp>

#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/codec/DataCodec.hpp>

#include <AvTranscoder/frame/Frame.hpp>

namespace avtranscoder
{

class AvExport IInputStream
{
public:
	virtual ~IInputStream() {};

	/**
	 * @brief Read the next packet of the stream
	 * @param data: data of next packet read
	 * @return if next packet was read succefully
	 **/
	virtual bool readNextPacket( CodedData& data ) = 0;

	/**
	 * @note The returned object could be cast depending on the type of the stream (video, audio...)
	 * @see VideoProperties, AudioProperties...
	 * @return the properties of the stream
	 */
	virtual const StreamProperties& getProperties() const = 0;

	/**
	 * @return the index of the stream
	 */
	virtual size_t getStreamIndex() const = 0;

	//@{
	/**
	 * Return the codec informations of the stream
	 * @exception Raise a runtime error if the stream is not of the corresponding type
         */
	virtual VideoCodec& getVideoCodec() = 0;
	virtual AudioCodec& getAudioCodec() = 0;
	virtual DataCodec& getDataCodec() = 0;
	//@}

	//@{
	/**
	 * @brief Functions about buffering
	 * Activate the stream will buffered its data when read packets.
	 * @see IInputStream methods
	 */
	virtual void activate( const bool activate = true ) = 0;
	virtual bool isActivated() const = 0;
	virtual void clearBuffering() = 0;
	//@}
};

}

#endif