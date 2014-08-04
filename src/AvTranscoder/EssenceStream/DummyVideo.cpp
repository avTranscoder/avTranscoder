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
	if( ! _inputFrame )
	{
		int fillChar = 0; // fill images with black
		
		if( frameBuffer.getSize() != _videoFrameDesc.getDataSize() )
			frameBuffer.getBuffer().resize( _videoFrameDesc.getDataSize() );
		memset( frameBuffer.getPtr(), fillChar, _videoFrameDesc.getDataSize() );
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
