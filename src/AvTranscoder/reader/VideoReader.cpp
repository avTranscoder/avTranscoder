#include "VideoReader.hpp"

#include <AvTranscoder/decoder/VideoDecoder.hpp>
#include <AvTranscoder/frame/VideoFrame.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/mediaProperty/print.hpp>

namespace avtranscoder
{

VideoReader::VideoReader( const std::string& filename, const size_t videoStreamIndex )
	: IReader( filename, videoStreamIndex )
	, _videoStreamProperties(NULL)
{
	init();
}

VideoReader::VideoReader( InputFile& inputFile, const size_t videoStreamIndex )
	: IReader( inputFile, videoStreamIndex )
	, _videoStreamProperties(NULL)
{
	init();
}

void VideoReader::init()
{
	// analyse InputFile
	avtranscoder::NoDisplayProgress p;
	_inputFile->analyse( p );
	_streamProperties = &_inputFile->getProperties().getStreamPropertiesWithIndex(_streamIndex);
	_videoStreamProperties = static_cast<const VideoProperties*>(_streamProperties);
	_inputFile->activateStream( _streamIndex );

	// setup decoder
	_decoder = new VideoDecoder( _inputFile->getStream( _streamIndex ) );
	_decoder->setupDecoder();

	// create src and dst frames
	_srcFrame = new VideoFrame( _inputFile->getStream( _streamIndex ).getVideoCodec().getVideoFrameDesc() );
	VideoFrame* srcFrame = static_cast<VideoFrame*>(_srcFrame);
	VideoFrameDesc videoFrameDescToDisplay( srcFrame->desc().getWidth(), srcFrame->desc().getHeight(), "rgb24" );
	_dstFrame = new VideoFrame( videoFrameDescToDisplay );

	// create transform
	_transform = new VideoTransform();
}

VideoReader::~VideoReader()
{
	delete _decoder;
	delete _srcFrame;
	delete _dstFrame;
	delete _transform;
}

size_t VideoReader::getWidth()
{
	return _videoStreamProperties->getWidth();
};

size_t VideoReader::getHeight()
{
	return _videoStreamProperties->getHeight();
}

size_t VideoReader::getComponents()
{
	return _videoStreamProperties->getPixelProperties().getNbComponents();
}

size_t VideoReader::getBitDepth()
{
	return _videoStreamProperties->getPixelProperties().getBitsPerPixel();
}

AVPixelFormat VideoReader::getPixelFormat()
{
	return _videoStreamProperties->getPixelProperties().getAVPixelFormat();
}

void VideoReader::printInfo()
{
	std::cout << *_videoStreamProperties << std::endl;
}

}
