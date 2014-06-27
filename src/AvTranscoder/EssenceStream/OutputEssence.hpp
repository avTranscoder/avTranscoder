#ifndef _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_ESSENCE_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_ESSENCE_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/pixfmt.h>
}

#include <string>
#include <vector>

#include <AvTranscoder/DatasStructures/Frame.hpp>
#include <AvTranscoder/DatasStructures/DataStreamDesc.hpp>

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
	 * @return status of setup
	 */
	virtual bool setup() = 0;

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

	/**
	 * @brief Set the profile for the encoder
	 * @note see Profile to get list of supported profiles
	 * @param profile selected profile name
	 */
	virtual void setProfile( const std::string& profile ) = 0;

};

}

#endif