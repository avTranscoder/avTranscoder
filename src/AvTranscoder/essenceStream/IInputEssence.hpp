#ifndef _AV_TRANSCODER_ESSENCE_STREAM_IINPUT_ESSENCE_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_IINPUT_ESSENCE_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/essenceStructures/Frame.hpp>

namespace avtranscoder
{

class AvInputStream;

class AvExport IInputEssence
{
public:
	virtual ~IInputEssence() {};
	
	virtual void setup() = 0;

	virtual bool readNextFrame( Frame& frameBuffer ) = 0;
	virtual bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex ) = 0;
};

}

#endif
