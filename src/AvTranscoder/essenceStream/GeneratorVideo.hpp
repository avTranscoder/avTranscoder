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

	// Stream properties
	void setVideoCodec( const VideoCodec& codec );

	const VideoCodec& getVideoCodec();
	
	void setup() {}

	void setFrame( Frame& inputFrame );
	
	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	const VideoCodec* _codec;
	Frame* _inputFrame;
	VideoFrameDesc _videoFrameDesc;

	size_t         _numberOfView;
};

}

#endif