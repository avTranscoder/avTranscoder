#include "GeneratorVideo.hpp"

#include <AvTranscoder/transform/VideoTransform.hpp>

namespace avtranscoder
{

GeneratorVideo::GeneratorVideo( )
	: IInputEssence( )
	, _inputFrame( NULL )
	, _codec( eCodecTypeEncoder )
	, _videoFrameDesc()
	, _numberOfView( 1 )
{
}

GeneratorVideo::~GeneratorVideo( )
{
}

void GeneratorVideo::setVideoCodec( const VideoCodec& codec )
{
	_codec = codec;
	_videoFrameDesc = _codec.getVideoFrameDesc();
}

VideoCodec& GeneratorVideo::getVideoCodec()
{
	return _codec;
}

void GeneratorVideo::setFrame( Frame& inputFrame )
{
	_inputFrame = &inputFrame;
}

bool GeneratorVideo::readNextFrame( Frame& frameBuffer )
{
	if( ! _inputFrame )
	{
		// @todo support PAL (0 to 255) and NTFS (16 to 235)
		int fillChar = 0;

		if( frameBuffer.getSize() != _videoFrameDesc.getDataSize() )
			frameBuffer.getBuffer().resize( _videoFrameDesc.getDataSize() );

		VideoFrameDesc desc( _codec.getVideoFrameDesc() );
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
