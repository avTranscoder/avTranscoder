#include "VideoGenerator.hpp"

#include <AvTranscoder/transform/VideoTransform.hpp>

namespace avtranscoder
{

VideoGenerator::VideoGenerator()
	: _inputFrame( NULL )
	, _frameDesc()
{
}

void VideoGenerator::setVideoFrameDesc( const VideoFrameDesc& frameDesc )
{
	_frameDesc = frameDesc;
}

void VideoGenerator::setNextFrame( Frame& inputFrame )
{
	_inputFrame = &inputFrame;
}

bool VideoGenerator::decodeNextFrame( Frame& frameBuffer )
{
	// Generate black image
	if( ! _inputFrame )
	{
		// @todo support PAL (0 to 255) and NTFS (16 to 235)
		char fillChar = 0;

		VideoFrameDesc desc( _frameDesc );
		desc.setPixelFormat( "rgb24" );

		VideoFrame intermediateBuffer( desc );
		intermediateBuffer.getBuffer().resize( _frameDesc.getDataSize() );
		memset( intermediateBuffer.getPtr(), fillChar, _frameDesc.getDataSize() );

		VideoTransform videoEssenceTransform;
		videoEssenceTransform.convert( intermediateBuffer, frameBuffer );
	}
	// Take image from _inputFrame
	else
	{
		frameBuffer.copyData( _inputFrame->getPtr(), _inputFrame->getSize() );
	}
	return true;
}

bool VideoGenerator::decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

}
