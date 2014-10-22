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

	~GeneratorVideo( );

	// Stream properties
	void setVideoCodec( const VideoCodec& codec );

	VideoCodec& getVideoCodec();
	
	void setup() {}

	void setFrame( Frame& inputFrame );
	
	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	Frame*         _inputFrame;
	VideoCodec      _codec;
	VideoFrameDesc _videoFrameDesc;

	size_t         _numberOfView;
};

}

#endif