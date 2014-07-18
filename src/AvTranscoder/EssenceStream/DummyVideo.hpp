#ifndef _AV_TRANSCODER_ESSENCE_STREAM_DUMMY_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_DUMMY_VIDEO_HPP_

#include "InputEssence.hpp"

#include <AvTranscoder/DatasStructures/VideoDesc.hpp>
#include <AvTranscoder/DatasStructures/Frame.hpp>

namespace avtranscoder
{

class AvExport DummyVideo : public InputEssence
{
public:
	DummyVideo( );

	~DummyVideo( );

	// Stream properties
	void setVideoDesc( VideoDesc& videoDesc );

	VideoDesc getVideoDesc() const;
	
	void setup() {}

	void setFrame( Frame& inputFrame );
	
	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	Frame*    _inputFrame;
	VideoDesc _videoDesc;
	ImageDesc _imageDesc;

	size_t    _numberOfView;
};

}

#endif