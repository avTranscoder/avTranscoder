#ifndef _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_VIDEO_HPP_

#include "IInputEssence.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>

namespace avtranscoder
{

class AvExport GeneratorVideo : public IInputEssence
{
public:
	GeneratorVideo( );

	VideoFrameDesc& getVideoFrameDesc() { return _frameDesc; }
	void setVideoFrameDesc( const VideoFrameDesc& frameDesc );
	
	void setup() {}

	void setFrame( Frame& inputFrame );
	
	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	Frame* _inputFrame;  ///< Has link (no ownership)
	VideoFrameDesc _frameDesc;
};

}

#endif