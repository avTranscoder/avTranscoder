#ifndef _AV_TRANSCODER_DATA_FRAME_HPP_
#define _AV_TRANSCODER_DATA_FRAME_HPP_

#include <AvTranscoder/common.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

typedef std::vector< unsigned char> DataBuffer;

class AvExport Frame
{
public:
	Frame(){};
	
	virtual ~Frame(){};

	virtual DataBuffer& getBuffer() { return m_dataBuffer; }
	virtual unsigned char* getPtr() { return &m_dataBuffer[0]; }
#ifndef SWIG
	virtual const unsigned char* getPtr() const { return &m_dataBuffer[0]; }
#endif
	virtual size_t getSize() const { return m_dataBuffer.size(); }
	
protected:
	DataBuffer m_dataBuffer;

};

}

#endif
