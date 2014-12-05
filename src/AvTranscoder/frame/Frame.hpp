#ifndef _AV_TRANSCODER_DATA_FRAME_HPP_
#define _AV_TRANSCODER_DATA_FRAME_HPP_

#include <AvTranscoder/common.hpp>

#include <vector>

namespace avtranscoder
{

typedef std::vector< unsigned char > DataBuffer;

class AvExport Frame
{
public:
	Frame(){};

	~Frame(){};

	void copyData(unsigned char* buffer, const size_t size)
	{
		_dataBuffer.resize( size );
		if ( size != 0 )
			memcpy( getPtr(), buffer, size );
	}

	DataBuffer& getBuffer() { return _dataBuffer; }
	unsigned char* getPtr() { return &_dataBuffer[0]; }

#ifndef SWIG
	const unsigned char* getPtr() const { return &_dataBuffer[0]; }
#endif

	size_t getSize() const { return _dataBuffer.size(); }

protected:
	DataBuffer _dataBuffer;
};

// Typedef to represent buffer of coded data.
// Example 1: in case of image, no sense to get size if coded data.
// Example 2: in case of audio, no sense to get number of channels if coded data.
typedef Frame CodedData;

}

#endif
