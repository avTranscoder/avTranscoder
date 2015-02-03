#include "VideoGenerator.hpp"

#include <AvTranscoder/transform/VideoTransform.hpp>

namespace avtranscoder
{

VideoGenerator::VideoGenerator()
	: _inputFrame( NULL )
	, _blackImage( NULL )
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
		// Generate the black image only once
		if( ! _blackImage )
		{
			// @todo support PAL (0 to 255) and NTFS (16 to 235)
			char fillChar = 0;

			// input of convert
			VideoFrameDesc desc( _frameDesc );
			desc.setPixelFormat( "rgb24" );

			VideoFrame intermediateBuffer( desc );
			intermediateBuffer.assign( _frameDesc.getDataSize(), fillChar );

			// output of convert
			VideoFrame& imageBuffer = static_cast<VideoFrame&>( frameBuffer );
			_blackImage = new VideoFrame( imageBuffer.desc() );

			// convert and store the black image
			VideoTransform videoTransform;
			videoTransform.convert( intermediateBuffer, *_blackImage );
		}
		frameBuffer.refData( *_blackImage );
	}
	// Take image from _inputFrame
	else
	{
		frameBuffer.refData( _inputFrame->getData(), _inputFrame->getSize() );
	}
	return true;
}

bool VideoGenerator::decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

}
