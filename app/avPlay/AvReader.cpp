#include "AvReader.hpp"

#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/mediaProperty/print.hpp>

AvReader::AvReader( const std::string& filename )
	: _inputFile( filename )
	, _inputVideo( NULL )
	, _sourceImage( NULL )
	, _imageToDisplay( NULL )
	, _videoTransform()
	, _videoStream( 0 )
{
	avtranscoder::NoDisplayProgress p;
	_inputFile.analyse( p );
	_videoStream = _inputFile.getProperties().getVideoProperties().at(0).getStreamId();
	_inputFile.activateStream( _videoStream );

	_inputVideo = new avtranscoder::VideoDecoder( _inputFile.getStream( _videoStream ) );
	_inputVideo->setup();

	_sourceImage = new avtranscoder::VideoFrame( _inputFile.getStream( _videoStream ).getVideoCodec().getVideoFrameDesc() );

	avtranscoder::VideoFrameDesc videoFrameDescToDisplay( _sourceImage->desc().getWidth(), _sourceImage->desc().getHeight(), "rgb24" );
	_imageToDisplay = new avtranscoder::VideoFrame( videoFrameDescToDisplay );
}

AvReader::~AvReader()
{
	delete _inputVideo;
	delete _sourceImage;
	delete _imageToDisplay;
}

size_t AvReader::getWidth()
{
	return _inputFile.getProperties().getVideoProperties().at(0).getWidth();
};

size_t AvReader::getHeight()
{
	return _inputFile.getProperties().getVideoProperties().at(0).getHeight();
}

size_t AvReader::getComponents()
{
	return _inputFile.getProperties().getVideoProperties().at(0).getPixelProperties().getNbComponents();
}

size_t AvReader::getBitDepth()
{
	return _inputFile.getProperties().getVideoProperties().at(0).getPixelProperties().getBitsPerPixel();
}

AVPixelFormat AvReader::getPixelFormat()
{
	return _inputFile.getProperties().getVideoProperties().at(0).getPixelProperties().getAVPixelFormat();
}

const char* AvReader::readNextFrame()
{
	++_currentFrame;
	_inputVideo->decodeNextFrame( *_sourceImage );
	_videoTransform.convert( *_sourceImage, *_imageToDisplay );
	return (const char*)_imageToDisplay->getData();
}

const char* AvReader::readPrevFrame()
{
	--_currentFrame;
	return readFrameAt( _currentFrame );
}

const char* AvReader::readFrameAt( const size_t frame )
{
	// /std::cout << "seek at " << frame << std::endl;
	_inputFile.seekAtFrame( frame );
	_inputVideo->flushDecoder();
	return readNextFrame();
}

void AvReader::printMetadatas()
{
	std::cout << _inputFile << std::endl;
}
