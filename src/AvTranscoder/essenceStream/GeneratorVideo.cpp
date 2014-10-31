#include "GeneratorVideo.hpp"

#include <AvTranscoder/transform/VideoTransform.hpp>

#include <stdexcept>

namespace avtranscoder
{

GeneratorVideo::GeneratorVideo( )
	: IInputEssence( )
	, _codec( NULL )
	, _inputFrame( NULL )
	, _videoFrameDesc()
	, _numberOfView( 1 )
{
}

void GeneratorVideo::setVideoCodec( const VideoCodec& codec )
{
	_codec = &codec;
	_videoFrameDesc = _codec->getVideoFrameDesc();
}

const VideoCodec& GeneratorVideo::getVideoCodec()
{
	return *_codec;
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
		if( ! _codec )
		{
			throw std::runtime_error( "Can't readNextFrame of video generator without knowing codec." );
		}

		// @todo support PAL (0 to 255) and NTFS (16 to 235)
		int fillChar = 0;

		if( frameBuffer.getSize() != _videoFrameDesc.getDataSize() )
			frameBuffer.getBuffer().resize( _videoFrameDesc.getDataSize() );

		VideoFrameDesc desc( _codec->getVideoFrameDesc() );
		Pixel rgbPixel;
		rgbPixel.setColorComponents( eComponentRgb );
		rgbPixel.setPlanar( false );
		desc.setPixel( rgbPixel );

		VideoFrame intermediateBuffer( desc );
		intermediateBuffer.getBuffer().resize( _videoFrameDesc.getDataSize() );
		memset( intermediateBuffer.getPtr(), fillChar, _videoFrameDesc.getDataSize() );

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
