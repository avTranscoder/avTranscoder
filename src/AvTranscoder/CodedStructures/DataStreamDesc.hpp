#ifndef _AV_TRANSCODER_DATA_DATA_STREAM_DESC_HPP_
#define _AV_TRANSCODER_DATA_DATA_STREAM_DESC_HPP_

#include <string>
#include <vector>

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

class DataStream
{
public:
	typedef std::vector< unsigned char > DataBuffer;

	DataStream( )
		: m_dataBuffer( 0, 0 )
	{ }

	DataBuffer&           getBuffer()     { return m_dataBuffer; }
	unsigned char*        getPtr()        { return &m_dataBuffer[0]; }
#ifndef SWIG
	const unsigned char*  getPtr()  const { return &m_dataBuffer[0]; }
#endif
	size_t                getSize() const { return m_dataBuffer.size(); }

private:
	DataBuffer m_dataBuffer;
};

}

#endif