#ifndef _MEDIA_ENGINE_IO_ESSENCE_STREAM_GENERATOR_VIDEO_HPP_
#define _MEDIA_ENGINE_IO_ESSENCE_STREAM_GENERATOR_VIDEO_HPP_

#include "IInputEssence.hpp"
#include <mediaIO/codec/VideoCodec.hpp>

namespace mediaengine
{
namespace mediaio
{

class MediaEngineExport GeneratorVideo : public IInputEssence
{
public:
	GeneratorVideo( );

	// Stream properties
	void setVideoCodec( const VideoCodec& codec );

	const VideoCodec& getVideoCodec();
	
	void setup() {}

	void setFrame( mediacore::Frame& inputFrame );
	
	bool readNextFrame( mediacore::Frame& frameBuffer );
	bool readNextFrame( mediacore::Frame& frameBuffer, const size_t subStreamIndex );

private:
	const VideoCodec*          _codec;
	mediacore::Frame*          _inputFrame;
	mediacore::VideoFrameDesc  _videoFrameDesc;

	size_t                     _numberOfView;
};

}
}

#endif