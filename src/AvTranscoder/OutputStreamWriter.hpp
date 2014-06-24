#ifndef _AV_TRANSCODER_OUTPUT_STREAM_WRITER_HPP_
#define _AV_TRANSCODER_OUTPUT_STREAM_WRITER_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/pixfmt.h>
}

#include <string>
#include <vector>

#include "DatasStructures/Frame.hpp"
#include "DatasStructures/DataStreamDesc.hpp"

namespace avtranscoder
{

class AvExport OutputStreamWriter
{
public:
	OutputStreamWriter()
	{}

	virtual ~OutputStreamWriter()
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