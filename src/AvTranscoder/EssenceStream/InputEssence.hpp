#ifndef _AV_TRANSCODER_ESSENCE_STREAM_INPUT_ESSENCE_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_INPUT_ESSENCE_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/DatasStructures/Frame.hpp>

namespace avtranscoder
{

class AvInputStream;

class AvExport InputEssence
{
public:
	InputEssence( AvInputStream& inputStream ) {};

	virtual ~InputEssence() {};
	
	virtual void setup() = 0;

	virtual bool readNextFrame( Frame& frameBuffer ) = 0;
};

}

#endif
