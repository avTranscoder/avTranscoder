#include "DummyVideo.hpp"

#include <cassert>
#include <cstring>

namespace avtranscoder
{

DummyVideo::DummyVideo( )
	: InputEssence( )
	, _inputFrame( NULL )
	, _numberOfView( 1 )
{
}

DummyVideo::~DummyVideo( )
{
}

void DummyVideo::setVideoDesc( const VideoDesc& videoDesc )
{
	_videoDesc = videoDesc;
	_imageDesc = _videoDesc.getImageDesc();
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
	frameBuffer.getBuffer().resize( _imageDesc.getDataSize() );

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
