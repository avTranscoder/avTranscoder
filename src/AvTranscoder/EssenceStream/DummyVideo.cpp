#include "DummyVideo.hpp"

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
	frameBuffer.getBuffer().resize( _videoFrameDesc.getDataSize() );

	if( ! _inputFrame )
	{
		int fillChar = 0; // fill images with black
		memset( frameBuffer.getPtr(), fillChar, frameBuffer.getSize() );
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
