#include "DummyVideo.hpp"

#include <cassert>
#include <cstring>

namespace avtranscoder
{

DummyVideo::DummyVideo( )
	: InputEssence( )
	, _numberOfView( 1 )
{
}

DummyVideo::~DummyVideo( )
{
}

void DummyVideo::setVideoDesc( VideoDesc& videoDesc )
{
	_videoDesc = videoDesc;
	_imageDesc = _videoDesc.getImageDesc();
}

VideoDesc DummyVideo::getVideoDesc() const
{
	return _videoDesc;
}

bool DummyVideo::readNextFrame( Frame& frameBuffer )
{
	frameBuffer.getBuffer().resize( _imageDesc.getDataSize() );

	int fillChar = 0; // fill images with black
	memset( frameBuffer.getPtr(), fillChar, frameBuffer.getSize() );

	return true;
}

bool DummyVideo::readNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

}
