#ifndef _AV_TRANSCODER_IPUT_STREAM_READER_HPP_
#define _AV_TRANSCODER_IPUT_STREAM_READER_HPP_

#include "DatasStructures/Frame.hpp"

namespace avtranscoder
{

class AvInputStream;

class AvExport InputStreamReader
{
public:
	InputStreamReader( AvInputStream& inputStream ) {};

	virtual ~InputStreamReader() {};
	
	virtual void setup() = 0;

	virtual bool readNextFrame( Frame& frameBuffer ) = 0;
};

}

#endif
