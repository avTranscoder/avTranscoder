#include "VideoReader.hpp"

#include <AvTranscoder/decoder/VideoDecoder.hpp>
#include <AvTranscoder/frame/VideoFrame.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/properties/print.hpp>

namespace avtranscoder
{

VideoReader::VideoReader(const std::string& filename, const size_t videoStreamIndex, const size_t width, const size_t height,
                         const std::string& pixelFormat)
    : IReader(filename, videoStreamIndex)
    , _videoStreamProperties(NULL)
    , _width(width)
    , _height(height)
    , _pixelProperties(pixelFormat)
{
    init();
}

VideoReader::VideoReader(InputFile& inputFile, const size_t videoStreamIndex, const size_t width, const size_t height,
                         const std::string& pixelFormat)
    : IReader(inputFile, videoStreamIndex)
    , _videoStreamProperties(NULL)
    , _width(width)
    , _height(height)
    , _pixelProperties(pixelFormat)
{
    init();
}

void VideoReader::init()
{
    // analyse InputFile
    avtranscoder::NoDisplayProgress p;
    _inputFile->analyse(p);
    _streamProperties = &_inputFile->getProperties().getStreamPropertiesWithIndex(_streamIndex);
    _videoStreamProperties = static_cast<const VideoProperties*>(_streamProperties);
    _inputFile->activateStream(_streamIndex);

    // setup decoder
    _decoder = new VideoDecoder(_inputFile->getStream(_streamIndex));
    _decoder->setupDecoder();

    // create src frame
    _srcFrame = new VideoFrame(_inputFile->getStream(_streamIndex).getVideoCodec().getVideoFrameDesc());
    VideoFrame* srcFrame = static_cast<VideoFrame*>(_srcFrame);
    // create dst frame
    if(_width == 0)
        _width = srcFrame->desc().getWidth();
    if(_height == 0)
        _height = srcFrame->desc().getHeight();
    VideoFrameDesc videoFrameDescToDisplay(_width, _height, getPixelFormat());
    _dstFrame = new VideoFrame(videoFrameDescToDisplay);

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
    return _width;
};

size_t VideoReader::getHeight()
{
    return _height;
}

size_t VideoReader::getComponents()
{
    return _pixelProperties.getNbComponents();
}

size_t VideoReader::getBitDepth()
{
    return _pixelProperties.getBitsPerPixel();
}

AVPixelFormat VideoReader::getPixelFormat()
{
    return _pixelProperties.getAVPixelFormat();
}

void VideoReader::printInfo()
{
    std::cout << *_videoStreamProperties << std::endl;
}
}
