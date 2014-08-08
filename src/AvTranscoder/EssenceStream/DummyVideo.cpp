#include "DummyVideo.hpp"

#include <AvTranscoder/EssenceTransform/VideoEssenceTransform.hpp>

namespace avtranscoder
{

DummyVideo::DummyVideo( )
	: InputEssence( )
	, _inputFrame( NULL )
	, _videoDesc()
	, _videoFrameDesc()
	, _numberOfView( 1 )
{
}

DummyVideo::~DummyVideo( )
{
}

void DummyVideo::setVideoDesc( const VideoDesc& videoDesc )
{
	_videoDesc = videoDesc;
	_videoFrameDesc = _videoDesc.getVideoFrameDesc();
}

VideoDesc DummyVideo::getVideoDesc() const
{
	return _videoDesc;
}

void DummyVideo::setFrame( Frame& inputFrame )
{
	_inputFrame = &inputFrame;
}

bool DummyVideo::readNextFrame( Frame& frameBuffer )
{
	if( ! _inputFrame )
	{
		// @todo support PAL (0 to 255) and NTFC (16 to 235)
		int fillChar = 0;

		if( frameBuffer.getSize() != _videoFrameDesc.getDataSize() )
			frameBuffer.getBuffer().resize( _videoFrameDesc.getDataSize() );

		VideoFrameDesc desc( _videoDesc.getVideoFrameDesc() );
		Pixel rgbPixel;
		rgbPixel.setColorComponents( eComponentRgb );
		rgbPixel.setPlanar( false );
		desc.setPixel( rgbPixel );

		VideoFrame intermediateBuffer( desc );
		intermediateBuffer.getBuffer().resize( _videoFrameDesc.getDataSize() );
		memset( intermediateBuffer.getPtr(), fillChar, _videoFrameDesc.getDataSize() );

		VideoEssenceTransform videoEssenceTransform;
		videoEssenceTransform.convert( intermediateBuffer, frameBuffer );

		return true;
	}
	
	if( frameBuffer.getSize() != _inputFrame->getSize() )
		frameBuffer.getBuffer().resize( _inputFrame->getSize() );
	std::memcpy( frameBuffer.getPtr(), _inputFrame->getPtr(), _inputFrame->getSize() );
	return true;
}

bool DummyVideo::readNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

}
