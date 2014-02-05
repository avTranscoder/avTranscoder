#ifndef _AV_TRANSCODER_DATA_VIDEO_STREAM_HPP_
#define _AV_TRANSCODER_DATA_VIDEO_STREAM_HPP_

#include "Image.hpp"
#include <string>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#ifndef INT64_C
	#define INT64_C(c) (c ## LL)
	#define UINT64_C(c) (c ## ULL)
#endif
#include <libavcodec/avcodec.h>
}

namespace avtranscoder
{

class VideoStream
{
public:
	VideoStream(){};

	void setCodecFromName( const std::string& codecName );

	void setCodecFromID( const AVCodecID codecId ) { m_codecId = codecId; }

	void setParametersFromImage( const Image& image );

	void setBitrate( const size_t bitRate ) { m_bitRate = bitRate; }

	AVCodecID getCodecId() const { return m_codecId; }

private:
	AVCodecID m_codecId;
	size_t    m_bitRate;
};

}

#endif