#ifndef _AV_TRANSCODER_DECODER_VIDEO_GENERATOR_HPP_
#define _AV_TRANSCODER_DECODER_VIDEO_GENERATOR_HPP_

#include "IDecoder.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>

namespace avtranscoder
{

class AvExport VideoGenerator : public IDecoder
{
public:
	VideoGenerator();

	VideoFrameDesc& getVideoFrameDesc() { return _frameDesc; }
	void setVideoFrameDesc( const VideoFrameDesc& frameDesc );
	
	void setup() {}

	void setNextFrame( Frame& inputFrame );
	
	bool decodeNextFrame( Frame& frameBuffer );
	bool decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	Frame* _inputFrame;  ///< Has link (no ownership)
	VideoFrameDesc _frameDesc;
};

}

#endif