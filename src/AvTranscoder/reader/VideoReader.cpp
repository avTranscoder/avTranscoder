#include "VideoReader.hpp"

#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/mediaProperty/print.hpp>

namespace avtranscoder
{

VideoReader::VideoReader( const std::string& filename, const size_t videoStreamIndex )
	: _inputFile( filename )
	, _videoProperties( NULL )
	, _videoDecoder( NULL )
	, _sourceImage( NULL )
	, _imageToDisplay( NULL )
	, _videoTransform()
	, _videoStreamIndex( videoStreamIndex )
{
	avtranscoder::NoDisplayProgress p;
	_inputFile.analyse( p );
	_videoProperties = &static_cast<const VideoProperties&>(_inputFile.getProperties().getStreamPropertiesWithIndex(_videoStreamIndex));
	_inputFile.activateStream( _videoStreamIndex );

	_videoDecoder = new avtranscoder::VideoDecoder( _inputFile.getStream( _videoStreamIndex ) );
	_videoDecoder->setupDecoder();

	_sourceImage = new avtranscoder::VideoFrame( _inputFile.getStream( _videoStreamIndex ).getVideoCodec().getVideoFrameDesc() );

	avtranscoder::VideoFrameDesc videoFrameDescToDisplay( _sourceImage->desc().getWidth(), _sourceImage->desc().getHeight(), "rgb24" );
	_imageToDisplay = new avtranscoder::VideoFrame( videoFrameDescToDisplay );
}

VideoReader::~VideoReader()
{
	delete _videoDecoder;
	delete _sourceImage;
	delete _imageToDisplay;
}

size_t VideoReader::getWidth()
{
	return _videoProperties->getWidth();
};

size_t VideoReader::getHeight()
{
	return _videoProperties->getHeight();
}

size_t VideoReader::getComponents()
{
	return _videoProperties->getPixelProperties().getNbComponents();
}

size_t VideoReader::getBitDepth()
{
	return _videoProperties->getPixelProperties().getBitsPerPixel();
}

AVPixelFormat VideoReader::getPixelFormat()
{
	return _videoProperties->getPixelProperties().getAVPixelFormat();
}

const char* VideoReader::readNextFrame()
{
	++_currentFrame;
	_videoDecoder->decodeNextFrame( *_sourceImage );
	_videoTransform.convert( *_sourceImage, *_imageToDisplay );
	return (const char*)_imageToDisplay->getData();
}

const char* VideoReader::readPrevFrame()
{
	--_currentFrame;
	return readFrameAt( _currentFrame );
}

const char* VideoReader::readFrameAt( const size_t frame )
{
	// /std::cout << "seek at " << frame << std::endl;
	_inputFile.seekAtFrame( frame );
	_videoDecoder->flushDecoder();
	return readNextFrame();
}

void VideoReader::printInfo()
{
	std::cout << *_videoProperties << std::endl;
}

}
