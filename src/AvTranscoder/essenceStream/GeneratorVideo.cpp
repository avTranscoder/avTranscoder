#include "GeneratorVideo.hpp"

#include <AvTranscoder/transform/VideoTransform.hpp>

namespace avtranscoder
{

GeneratorVideo::GeneratorVideo( )
	: IInputEssence( )
	, _inputFrame( NULL )
	, _frameDesc()
{
}

void GeneratorVideo::setVideoFrameDesc( const VideoFrameDesc& frameDesc )
{
	_frameDesc = frameDesc;
}

void GeneratorVideo::setFrame( Frame& inputFrame )
{
	_inputFrame = &inputFrame;
}

bool GeneratorVideo::readNextFrame( Frame& frameBuffer )
{
	// Generate black image
	if( ! _inputFrame )
	{
		// @todo support PAL (0 to 255) and NTFS (16 to 235)
		int fillChar = 0;

		if( frameBuffer.getSize() != _frameDesc.getDataSize() )
			frameBuffer.getBuffer().resize( _frameDesc.getDataSize() );

		VideoFrameDesc desc( _frameDesc );
		Pixel rgbPixel;
		rgbPixel.setColorComponents( eComponentRgb );
		rgbPixel.setPlanar( false );
		desc.setPixel( rgbPixel );

		VideoFrame intermediateBuffer( desc );
		intermediateBuffer.getBuffer().resize( _frameDesc.getDataSize() );
		memset( intermediateBuffer.getPtr(), fillChar, _frameDesc.getDataSize() );

		VideoTransform videoEssenceTransform;
		videoEssenceTransform.convert( intermediateBuffer, frameBuffer );

		return true;
	}

	// Take image from _inputFrame
	if( frameBuffer.getSize() != _inputFrame->getSize() )
		frameBuffer.getBuffer().resize( _inputFrame->getSize() );
	std::memcpy( frameBuffer.getPtr(), _inputFrame->getPtr(), _inputFrame->getSize() );
	return true;
}

bool GeneratorVideo::readNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

}
