#ifndef _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_GENERATOR_VIDEO_HPP_

#include "IInputEssence.hpp"

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/CodedStructures/VideoDesc.hpp>
#include <AvTranscoder/EssenceStructures/Frame.hpp>

namespace avtranscoder
{

class AvExport GeneratorVideo : public IInputEssence
{
public:
	GeneratorVideo( );

	~GeneratorVideo( );

	// Stream properties
	void setVideoDesc( const VideoDesc& videoDesc );

	VideoDesc getVideoDesc() const;
	
	void setup() {}

	void setFrame( Frame& inputFrame );
	
	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	Frame*         _inputFrame;
	VideoDesc      _videoDesc;
	VideoFrameDesc _videoFrameDesc;

	size_t         _numberOfView;
};

}

#endif