#include "VideoReader.hpp"

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/decoder/VideoDecoder.hpp>
#include <AvTranscoder/decoder/VideoGenerator.hpp>
#include <AvTranscoder/data/decoded/VideoFrame.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>

namespace avtranscoder
{

VideoReader::VideoReader(const InputStreamDesc& inputDesc)
    : IReader(inputDesc)
    , _videoStreamProperties(NULL)
    , _outputWidth(0)
    , _outputHeight(0)
    , _outputPixelProperties("rgb24")
{
    init();
}

void VideoReader::init()
{
    // analyse InputFile
    avtranscoder::NoDisplayProgress p;
    _inputFile->analyse(p);
    _streamProperties = &_inputFile->getProperties().getStreamPropertiesWithIndex(_inputDesc._streamIndex);
    _videoStreamProperties = static_cast<const VideoProperties*>(_streamProperties);
    _inputFile->activateStream(_inputDesc._streamIndex);

    // setup decoder
    _decoder = new VideoDecoder(_inputFile->getStream(_inputDesc._streamIndex));
    _decoder->setupDecoder();
    _currentDecoder = _decoder;

    // create src frame
    const VideoFrameDesc srcFrameDesc = _inputFile->getStream(_inputDesc._streamIndex).getVideoCodec().getVideoFrameDesc();
    _srcFrame = new VideoFrame(srcFrameDesc, false);
    VideoFrame* srcFrame = static_cast<VideoFrame*>(_srcFrame);
    // create dst frame
    _outputWidth = srcFrame->getWidth();
    _outputHeight = srcFrame->getHeight();
    _dstFrame = new VideoFrame(VideoFrameDesc(_outputWidth, _outputHeight, getPixelFormatName(getOutputPixelFormat())));

    // generator
    _generator = new VideoGenerator(srcFrameDesc);

    // create transform
    _transform = new VideoTransform();
}

VideoReader::~VideoReader()
{
    delete _decoder;
    delete _generator;
    delete _srcFrame;
    delete _dstFrame;
    delete _transform;
}

void VideoReader::updateOutput(const size_t width, const size_t height, const std::string& pixelFormat)
{
    _outputWidth = width;
    _outputHeight = height;
    _outputPixelProperties = PixelProperties(pixelFormat);
    // update dst frame
    delete _dstFrame;
    _dstFrame = new VideoFrame(VideoFrameDesc(_outputWidth, _outputHeight, getPixelFormatName(getOutputPixelFormat())));
}
}
