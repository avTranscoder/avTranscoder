#ifndef _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_ESSENCE_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_ESSENCE_HPP_

#include <AvTranscoder/EssenceStructures/Frame.hpp>
#include <AvTranscoder/CodedStructures/DataStreamDesc.hpp>

namespace avtranscoder
{

class AvExport OutputEssence
{
public:
	OutputEssence()
	{}

	virtual ~OutputEssence()
	{}

	/**
	 * @brief Setup the encoder
	 */
	virtual void setup() = 0;

	/**
	 * @brief Encode a new frame, and get coded frame
	 * @param sourceFrame frame need to be encoded
	 * @param codedFrame data of the coded frame if present (first frames can be delayed)
	 * @return status of encoding
	 */
	virtual bool encodeFrame( const Frame& sourceFrame, DataStream& codedFrame ) = 0;

	/**
	 * @brief Get delayed encoded frames
	 * @param codedFrame data of the coded frame if present (first frames can be delayed)
	 * @return status of encoding
	 */
	virtual bool encodeFrame( DataStream& codedFrame ) = 0;

};

}

#endif