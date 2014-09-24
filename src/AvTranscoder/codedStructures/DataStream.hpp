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
		: _dataBuffer( 0, 0 )
	{ }

	DataBuffer&           getBuffer()     { return _dataBuffer; }
	unsigned char*        getPtr()        { return &_dataBuffer[0]; }
#ifndef SWIG
	const unsigned char*  getPtr()  const { return &_dataBuffer[0]; }
#endif
	size_t                getSize() const { return _dataBuffer.size(); }

private:
	DataBuffer _dataBuffer;
};

}

#endif