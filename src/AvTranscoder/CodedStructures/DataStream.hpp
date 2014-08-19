#ifndef _AV_TRANSCODER_CODED_STRUCTURES_DATA_STREAM_DESC_HPP_
#define _AV_TRANSCODER_CODED_STRUCTURES_DATA_STREAM_DESC_HPP_

#include <vector>
#include <cstring>

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