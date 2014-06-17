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
	{
	}

	virtual bool setup() = 0;

	/**
	 * @param[in]  sourceFrame
	 * @param[out] codedFrame
	 */
	virtual bool encodeFrame( const Frame& sourceFrame, DataStream& codedFrame ) = 0;

	/**
	 * get delayed encoded frames
	 */
	virtual bool encodeFrame( DataStream& codedFrame ) = 0;

	virtual void setProfile( const std::string& profile ) = 0;

};

}

#endif