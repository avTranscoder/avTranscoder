#ifndef _AV_TRANSCODER_STREAM_I_OUTPUT_STREAM_HPP_
#define _AV_TRANSCODER_STREAM_I_OUTPUT_STREAM_HPP_

#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/frame/Frame.hpp>

namespace avtranscoder
{

class AvExport IOutputStream
{
public:

	/**
	 * @brief define wrapping result status
	 **/
	enum EWrappingStatus
	{
		eWrappingSuccess = 0,
		eWrappingWaitingForData,
		eWrappingError,
	};

	virtual ~IOutputStream() {};

	virtual size_t getStreamIndex() const = 0;

	virtual EWrappingStatus wrap( const CodedData& data ) = 0;
};

}

#endif