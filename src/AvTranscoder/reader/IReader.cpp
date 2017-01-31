#include "IReader.hpp"

#include <cassert>

namespace avtranscoder
{

IReader::IReader(const InputStreamDesc& inputDesc)
    : _inputDesc(inputDesc)
    , _inputFile(NULL)
    , _streamProperties(NULL)
    , _decoder(NULL)
    , _generator(NULL)
    , _currentDecoder(NULL)
    , _srcFrame(NULL)
    , _dstFrame(NULL)
    , _transform(NULL)
    , _currentFrame(-1)
    , _continueWithGenerator(false)
{
    _inputFile = new InputFile(_inputDesc._filename);
}

IReader::~IReader()
{
    delete _inputFile;
}

IFrame* IReader::readNextFrame()
{
    return readFrameAt(_currentFrame + 1);
}

IFrame* IReader::readPrevFrame()
{
    return readFrameAt(_currentFrame - 1);
}

IFrame* IReader::readFrameAt(const size_t frame)
{
    assert(_currentDecoder != NULL);
    assert(_transform != NULL);
    assert(_srcFrame != NULL);
    assert(_dstFrame != NULL);

    // seek
    if((int)frame != _currentFrame + 1)
    {
        _inputFile->seekAtFrame(frame);
        _decoder->flushDecoder();
    }
    _currentFrame = frame;
    // decode
    bool decodingStatus = false;
    if(! _inputDesc._channelIndexArray.empty())
    {
        decodingStatus = _currentDecoder->decodeNextFrame(*_srcFrame, _inputDesc._channelIndexArray);
    }
    else
        decodingStatus = _currentDecoder->decodeNextFrame(*_srcFrame);
    // if decoding failed
    if(!decodingStatus)
    {
        // generate data (ie silence or black)
        if(_continueWithGenerator)
        {
            // allocate the frame since the process will continue with some generated data
            if(! _srcFrame->isDataAllocated())
                _srcFrame->allocateData();
            _currentDecoder = _generator;
            return readFrameAt(frame);
        }
        // or return NULL
        else
        {
            return NULL;
        }
    }
    // transform
    _transform->convert(*_srcFrame, *_dstFrame);
    return _dstFrame;
}
}
