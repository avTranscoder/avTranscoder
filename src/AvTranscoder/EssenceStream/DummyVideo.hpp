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

	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( std::vector<Frame>& frameBuffer );

private:
	VideoDesc _videoDesc;
	ImageDesc _imageDesc;

	size_t numberOfView;
};

}

#endif