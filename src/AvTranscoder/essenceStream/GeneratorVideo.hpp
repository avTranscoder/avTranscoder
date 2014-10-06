#ifndef _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_VIDEO_HPP_

#include "IInputEssence.hpp"

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/frame/Frame.hpp>

namespace avtranscoder
{

class AvExport GeneratorVideo : public IInputEssence
{
public:
	GeneratorVideo( );

	~GeneratorVideo( );

	// Stream properties
	void setVideoDesc( const VideoCodec& videoDesc );

	VideoCodec getVideoCodec() const;
	
	void setup() {}

	void setFrame( Frame& inputFrame );
	
	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	Frame*         _inputFrame;
	VideoCodec      _videoDesc;
	VideoFrameDesc _videoFrameDesc;

	size_t         _numberOfView;
};

}

#endif