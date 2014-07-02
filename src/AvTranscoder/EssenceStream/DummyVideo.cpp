#include "DummyVideo.hpp"

#include <cassert>
#include <cstring>

namespace avtranscoder
{

DummyVideo::DummyVideo( )
	: InputEssence( )
	, numberOfView( 1 )
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

bool DummyVideo::readNextFrame( std::vector<Frame>& frameBuffer )
{
	if( frameBuffer.size() != numberOfView )
	{
		frameBuffer.resize( numberOfView );
	}

	for( size_t view = 0; view < numberOfView; ++view )
	{
		readNextFrame( frameBuffer.at( view ) );
	}

	return true;
}

}
