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
	, _width( 0 )
	, _height( 0 )
	, _pixelProperties()
{
	init();
}

VideoReader::VideoReader( InputFile& inputFile, const size_t videoStreamIndex )
	: IReader( inputFile, videoStreamIndex )
	, _videoStreamProperties(NULL)
	, _width( 0 )
	, _height( 0 )
	, _pixelProperties()
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

	// create transform
	_transform = new VideoTransform();

	// create src frame
	_srcFrame = new VideoFrame( _inputFile->getStream( _streamIndex ).getVideoCodec().getVideoFrameDesc() );
	VideoFrame* srcFrame = static_cast<VideoFrame*>(_srcFrame);
	// create dst frame
	_width = srcFrame->desc().getWidth();
	_height = srcFrame->desc().getHeight();
	_pixelProperties = PixelProperties( "rgb24" );
	VideoFrameDesc videoFrameDescToDisplay( _width, _height, getPixelFormat() );
	_dstFrame = new VideoFrame( videoFrameDescToDisplay );
}

VideoReader::~VideoReader()
{
	delete _decoder;
	delete _srcFrame;
	delete _dstFrame;
	delete _transform;
}

void VideoReader::updateOutput(const size_t width, const size_t height, const std::string& pixelFormat)
{
	_width = width;
	_height = height;
	_pixelProperties = PixelProperties( pixelFormat );
	// update dst frame
	delete _dstFrame;
	_dstFrame = new VideoFrame( VideoFrameDesc( _width, _height, getPixelFormat() ) );
}

void VideoReader::printInfo()
{
	std::cout << *_videoStreamProperties << std::endl;
}

}
