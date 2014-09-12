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
	
	virtual ~Frame(){};

	virtual DataBuffer& getBuffer() { return _dataBuffer; }
	virtual unsigned char* getPtr() { return &_dataBuffer[0]; }
#ifndef SWIG
	virtual const unsigned char* getPtr() const { return &_dataBuffer[0]; }
#endif
	virtual size_t getSize() const { return _dataBuffer.size(); }
	
protected:
	DataBuffer _dataBuffer;

};

}

#endif
