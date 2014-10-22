#ifndef _AV_TRANSCODER_ESSENCE_STREAM_IOUTPUT_ESSENCE_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_IOUTPUT_ESSENCE_HPP_

#include <avtranscoder/frame/Frame.hpp>
#include <avtranscoder/codec/ICodec.hpp>

namespace avtranscoder
{

class AvExport IOutputEssence
{
public:
	virtual ~IOutputEssence() {}

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
	virtual bool encodeFrame( const Frame& sourceFrame, Frame& codedFrame ) = 0;

	/**
	 * @brief Get delayed encoded frames
	 * @param codedFrame data of the coded frame if present (first frames can be delayed)
	 * @return status of encoding
	 */
	virtual bool encodeFrame( Frame& codedFrame ) = 0;

	/**
	 * @brief Get codec used for encoding.
	 * @return a reference to the codec
	 */
	virtual ICodec& getCodec() = 0;
};

}

#endif