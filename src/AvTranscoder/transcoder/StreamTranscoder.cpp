
#include "StreamTranscoder.hpp"

#include <AvTranscoder/stream/InputStream.hpp>

#include <AvTranscoder/decoder/VideoDecoder.hpp>
#include <AvTranscoder/decoder/AudioDecoder.hpp>
#include <AvTranscoder/decoder/VideoGenerator.hpp>
#include <AvTranscoder/decoder/AudioGenerator.hpp>
#include <AvTranscoder/encoder/VideoEncoder.hpp>
#include <AvTranscoder/encoder/AudioEncoder.hpp>

#include <AvTranscoder/transform/AudioTransform.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>

#include <cassert>
#include <limits>
#include <sstream>

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder(IInputStream& inputStream, IOutputFile& outputFile, const float offset)
    : _inputStream(&inputStream)
    , _outputStream(NULL)
    , _sourceBuffer(NULL)
    , _frameBuffer(NULL)
    , _inputDecoder(NULL)
    , _generator(NULL)
    , _currentDecoder(NULL)
    , _outputEncoder(NULL)
    , _transform(NULL)
    , _subStreamIndex(-1)
    , _offset(offset)
    , _needToSwitchToGenerator(false)
{
    // create a re-wrapping case
    switch(_inputStream->getProperties().getStreamType())
    {
        case AVMEDIA_TYPE_VIDEO:
        {
            // output stream
            _outputStream = &outputFile.addVideoStream(_inputStream->getVideoCodec());

            try
            {
                VideoFrameDesc inputFrameDesc(_inputStream->getVideoCodec().getVideoFrameDesc());

                // generator decoder
                VideoGenerator* generatorVideo = new VideoGenerator();
                generatorVideo->setVideoFrameDesc(inputFrameDesc);
                _generator = generatorVideo;

                // buffers to process
                _sourceBuffer = new VideoFrame(inputFrameDesc);
                _frameBuffer = new VideoFrame(inputFrameDesc);

                // transform
                _transform = new VideoTransform();

                // output encoder
                VideoEncoder* outputVideo = new VideoEncoder(_inputStream->getVideoCodec().getCodecName());
                outputVideo->setupVideoEncoder(inputFrameDesc);
                _outputEncoder = outputVideo;
            }
            catch(std::runtime_error& e)
            {
                LOG_WARN("Cannot create the video encoder for stream " << _inputStream->getStreamIndex() << " if needed. "
                                                                       << e.what())
            }

            break;
        }
        case AVMEDIA_TYPE_AUDIO:
        {
            // output stream
            _outputStream = &outputFile.addAudioStream(_inputStream->getAudioCodec());

            try
            {
                AudioFrameDesc inputFrameDesc(_inputStream->getAudioCodec().getAudioFrameDesc());

                // generator decoder
                AudioGenerator* generatorAudio = new AudioGenerator();
                generatorAudio->setAudioFrameDesc(inputFrameDesc);
                _generator = generatorAudio;

                // buffers to process
                _sourceBuffer = new AudioFrame(inputFrameDesc);
                _frameBuffer = new AudioFrame(inputFrameDesc);

                // transform
                _transform = new AudioTransform();

                // output encoder
                AudioEncoder* outputAudio = new AudioEncoder(_inputStream->getAudioCodec().getCodecName());
                outputAudio->setupAudioEncoder(inputFrameDesc);
                _outputEncoder = outputAudio;
            }

            catch(std::runtime_error& e)
            {
                LOG_WARN("Cannot create the audio encoder for stream " << _inputStream->getStreamIndex() << " if needed. "
                                                                       << e.what())
            }

            break;
        }
        case AVMEDIA_TYPE_DATA:
        {
            // @warning: rewrap a data stream can't be lengthen by a generator (end of rewrapping will end the all process)
            _outputStream = &outputFile.addDataStream(_inputStream->getDataCodec());
            break;
        }
        default:
            break;
    }
    setOffset(offset);
}

StreamTranscoder::StreamTranscoder(IInputStream& inputStream, IOutputFile& outputFile, const ProfileLoader::Profile& profile,
                                   const int subStreamIndex, const float offset)
    : _inputStream(&inputStream)
    , _outputStream(NULL)
    , _sourceBuffer(NULL)
    , _frameBuffer(NULL)
    , _inputDecoder(NULL)
    , _generator(NULL)
    , _currentDecoder(NULL)
    , _outputEncoder(NULL)
    , _transform(NULL)
    , _subStreamIndex(subStreamIndex)
    , _offset(offset)
    , _needToSwitchToGenerator(false)
{
    // create a transcode case
    switch(_inputStream->getProperties().getStreamType())
    {
        case AVMEDIA_TYPE_VIDEO:
        {
            // input decoder
            VideoDecoder* inputVideo = new VideoDecoder(*static_cast<InputStream*>(_inputStream));
            inputVideo->setupDecoder();
            _inputDecoder = inputVideo;
            _currentDecoder = _inputDecoder;

            // output encoder
            VideoEncoder* outputVideo = new VideoEncoder(profile.at(constants::avProfileCodec));
            _outputEncoder = outputVideo;

            VideoFrameDesc outputFrameDesc = _inputStream->getVideoCodec().getVideoFrameDesc();
            outputFrameDesc.setParameters(profile);
            outputVideo->setupVideoEncoder(outputFrameDesc, profile);

            // output stream
            _outputStream = &outputFile.addVideoStream(outputVideo->getVideoCodec());

            // buffers to process
            _sourceBuffer = new VideoFrame(_inputStream->getVideoCodec().getVideoFrameDesc());
            _frameBuffer = new VideoFrame(outputVideo->getVideoCodec().getVideoFrameDesc());

            // transform
            _transform = new VideoTransform();

            // generator decoder
            VideoGenerator* generatorVideo = new VideoGenerator();
            generatorVideo->setVideoFrameDesc(outputVideo->getVideoCodec().getVideoFrameDesc());
            _generator = generatorVideo;

            break;
        }
        case AVMEDIA_TYPE_AUDIO:
        {
            // input decoder
            AudioDecoder* inputAudio = new AudioDecoder(*static_cast<InputStream*>(_inputStream));
            inputAudio->setupDecoder();
            _inputDecoder = inputAudio;
            _currentDecoder = _inputDecoder;

            // output encoder
            AudioEncoder* outputAudio = new AudioEncoder(profile.at(constants::avProfileCodec));
            _outputEncoder = outputAudio;

            AudioFrameDesc outputFrameDesc(_inputStream->getAudioCodec().getAudioFrameDesc());
            outputFrameDesc.setParameters(profile);
            if(subStreamIndex > -1)
            {
                // @todo manage downmix ?
                outputFrameDesc._nbChannels = 1;
            }
            outputAudio->setupAudioEncoder(outputFrameDesc, profile);

            // output stream
            _outputStream = &outputFile.addAudioStream(outputAudio->getAudioCodec());

            // buffers to process
            AudioFrameDesc inputFrameDesc(_inputStream->getAudioCodec().getAudioFrameDesc());
            if(subStreamIndex > -1)
                inputFrameDesc._nbChannels = 1;

            _sourceBuffer = new AudioFrame(inputFrameDesc);
            _frameBuffer = new AudioFrame(outputAudio->getAudioCodec().getAudioFrameDesc());

            // transform
            _transform = new AudioTransform();

            // generator decoder
            AudioGenerator* generatorAudio = new AudioGenerator();
            generatorAudio->setAudioFrameDesc(outputAudio->getAudioCodec().getAudioFrameDesc());
            _generator = generatorAudio;

            break;
        }
        default:
        {
            throw std::runtime_error("unupported stream type");
            break;
        }
    }
    setOffset(offset);
}

StreamTranscoder::StreamTranscoder(const ICodec& inputCodec, IOutputFile& outputFile, const ProfileLoader::Profile& profile)
    : _inputStream(NULL)
    , _outputStream(NULL)
    , _sourceBuffer(NULL)
    , _frameBuffer(NULL)
    , _inputDecoder(NULL)
    , _generator(NULL)
    , _currentDecoder(NULL)
    , _outputEncoder(NULL)
    , _transform(NULL)
    , _subStreamIndex(-1)
    , _offset(0)
    , _needToSwitchToGenerator(false)
{
    if(profile.find(constants::avProfileType)->second == constants::avProfileTypeVideo)
    {
        // generator decoder
        VideoGenerator* generatorVideo = new VideoGenerator();
        const VideoCodec& inputVideoCodec = static_cast<const VideoCodec&>(inputCodec);
        generatorVideo->setVideoFrameDesc(inputVideoCodec.getVideoFrameDesc());
        _generator = generatorVideo;
        _currentDecoder = _generator;

        // buffers to process
        VideoFrameDesc inputFrameDesc = inputVideoCodec.getVideoFrameDesc();
        VideoFrameDesc outputFrameDesc = inputFrameDesc;
        outputFrameDesc.setParameters(profile);
        _sourceBuffer = new VideoFrame(inputFrameDesc);
        _frameBuffer = new VideoFrame(outputFrameDesc);

        // transform
        _transform = new VideoTransform();

        // output encoder
        VideoEncoder* outputVideo = new VideoEncoder(profile.at(constants::avProfileCodec));
        outputVideo->setupVideoEncoder(outputFrameDesc, profile);
        _outputEncoder = outputVideo;

        // output stream
        _outputStream = &outputFile.addVideoStream(outputVideo->getVideoCodec());
    }
    else if(profile.find(constants::avProfileType)->second == constants::avProfileTypeAudio)
    {
        // generator decoder
        AudioGenerator* generatorAudio = new AudioGenerator();
        const AudioCodec& inputAudioCodec = static_cast<const AudioCodec&>(inputCodec);
        generatorAudio->setAudioFrameDesc(inputAudioCodec.getAudioFrameDesc());
        _generator = generatorAudio;
        _currentDecoder = _generator;

        // buffers to process
        AudioFrameDesc inputFrameDesc = inputAudioCodec.getAudioFrameDesc();
        AudioFrameDesc outputFrameDesc = inputFrameDesc;
        outputFrameDesc.setParameters(profile);
        _sourceBuffer = new AudioFrame(inputFrameDesc);
        _frameBuffer = new AudioFrame(outputFrameDesc);

        // transform
        _transform = new AudioTransform();

        // output encoder
        AudioEncoder* outputAudio = new AudioEncoder(profile.at(constants::avProfileCodec));
        outputAudio->setupAudioEncoder(outputFrameDesc, profile);
        _outputEncoder = outputAudio;

        // output stream
        _outputStream = &outputFile.addAudioStream(outputAudio->getAudioCodec());
    }
    else
    {
        throw std::runtime_error("unupported stream type");
    }
}

StreamTranscoder::~StreamTranscoder()
{
    delete _sourceBuffer;
    delete _frameBuffer;
    delete _generator;
    delete _outputEncoder;
    delete _transform;
    delete _inputDecoder;
}

void StreamTranscoder::preProcessCodecLatency()
{
    if(!_outputEncoder)
    {
        std::stringstream msg;
        msg << "No encoder found for input stream ";
        if(getProcessCase() == eProcessCaseGenerator)
            msg << "generator";
        else
            msg << _inputStream->getStreamIndex();
        msg << ": will not preProcessCodecLatency.";
        LOG_WARN(msg.str())
        return;
    }

    int latency = _outputEncoder->getCodec().getLatency();

    LOG_DEBUG("Latency of stream: " << latency)

    if(!latency || latency < _outputEncoder->getCodec().getAVCodecContext().frame_number)
        return;

    // set a decoder to preload generated frames
    bool wasARewrapCase = false;
    if(getProcessCase() == eProcessCaseRewrap)
    {
        switchToGeneratorDecoder();
        wasARewrapCase = true;
    }

    while((latency--) > 0)
    {
        processFrame();
    }

    if(wasARewrapCase)
        _currentDecoder = NULL;
}

bool StreamTranscoder::processFrame()
{
    const EProcessCase processCase = getProcessCase();
    std::string msg = "Current process case of the stream is a ";
    switch(processCase)
    {
        case eProcessCaseTranscode:
            msg += "transcode.";
            break;
        case eProcessCaseRewrap:
            msg += "rewrap.";
            break;
        case eProcessCaseGenerator:
            msg += "generator.";
            break;
    }
    LOG_DEBUG(msg)

    if(processCase == eProcessCaseGenerator)
        return processTranscode();

    // Manage offset
    if(_offset > 0)
    {
        const bool endOfOffset = _outputStream->getStreamDuration() >= _offset;
        if(endOfOffset)
        {
            LOG_INFO("End of positive offset")

            if(getProcessCase() == eProcessCaseTranscode)
                switchToInputDecoder();
            else
                _currentDecoder = NULL;
            _offset = 0;
        }
        else
        {
            // process generator
            if(_currentDecoder != _generator)
            {
                switchToGeneratorDecoder();
            }
        }
    }
    else if(_offset < 0)
    {
        const bool endOfStream =
            _outputStream->getStreamDuration() >= (_inputStream->getProperties().getDuration() + _offset);
        if(endOfStream)
        {
            LOG_INFO("End of negative offset")

            switchToGeneratorDecoder();
            _offset = 0;
        }
    }

    if(processCase == eProcessCaseRewrap)
        return processRewrap();

    return processTranscode(_subStreamIndex);
}

bool StreamTranscoder::processRewrap()
{
    assert(_inputStream != NULL);
    assert(_outputStream != NULL);
    assert(_inputDecoder == NULL);

    LOG_DEBUG("StreamTranscoder::processRewrap")

    // if switched to generator, process frame
    if(_currentDecoder && _currentDecoder == _generator)
    {
        return processTranscode();
    }

    CodedData data;
    if(!_inputStream->readNextPacket(data))
    {
        if(_needToSwitchToGenerator)
        {
            switchToGeneratorDecoder();
            return processTranscode();
        }
        return false;
    }

    const IOutputStream::EWrappingStatus wrappingStatus = _outputStream->wrap(data);
    switch(wrappingStatus)
    {
        case IOutputStream::eWrappingSuccess:
            return true;
        case IOutputStream::eWrappingWaitingForData:
            // the wrapper needs more data to write the current packet
            return processFrame();
        case IOutputStream::eWrappingError:
            return false;
    }

    return true;
}

bool StreamTranscoder::processTranscode(const int subStreamIndex)
{
    assert(_outputStream != NULL);
    assert(_currentDecoder != NULL);
    assert(_outputEncoder != NULL);
    assert(_sourceBuffer != NULL);
    assert(_frameBuffer != NULL);
    assert(_transform != NULL);

    LOG_DEBUG("StreamTranscoder::processTranscode")

    LOG_DEBUG("Decode next frame")
    bool decodingStatus = false;
    if(subStreamIndex < 0)
        decodingStatus = _currentDecoder->decodeNextFrame(*_sourceBuffer);
    else
        decodingStatus = _currentDecoder->decodeNextFrame(*_sourceBuffer, subStreamIndex);

    CodedData data;
    if(decodingStatus)
    {
        LOG_DEBUG("Convert")
        _transform->convert(*_sourceBuffer, *_frameBuffer);

        LOG_DEBUG("Encode")
        _outputEncoder->encodeFrame(*_frameBuffer, data);
    }
    else
    {
        LOG_DEBUG("Encode last frame(s)")
        if(!_outputEncoder->encodeFrame(data))
        {
            if(_needToSwitchToGenerator)
            {
                switchToGeneratorDecoder();
                return processTranscode();
            }
            return false;
        }
    }

    LOG_DEBUG("wrap (" << data.getSize() << " bytes)")
    const IOutputStream::EWrappingStatus wrappingStatus = _outputStream->wrap(data);
    switch(wrappingStatus)
    {
        case IOutputStream::eWrappingSuccess:
            return true;
        case IOutputStream::eWrappingWaitingForData:
            // the wrapper needs more data to write the current packet
            return processFrame();
        case IOutputStream::eWrappingError:
            return false;
    }

    return true;
}

void StreamTranscoder::switchToGeneratorDecoder()
{
    LOG_INFO("Switch to generator decoder")

    _currentDecoder = _generator;
    assert(_currentDecoder != NULL);
}

void StreamTranscoder::switchToInputDecoder()
{
    LOG_INFO("Switch to input decoder")

    _currentDecoder = _inputDecoder;
    assert(_currentDecoder != NULL);
}

float StreamTranscoder::getDuration() const
{
    if(_inputStream)
    {
        const StreamProperties& streamProperties = _inputStream->getProperties();
        const float totalDuration = streamProperties.getDuration() + _offset;
        if(totalDuration < 0)
        {
            LOG_WARN("Offset of " << _offset << "s applied to a stream with a duration of " << streamProperties.getDuration()
                                  << "s. Set its duration to 0s.")
            return 0.;
        }
        return totalDuration;
    }
    // generator
    else
        return std::numeric_limits<float>::max();
}

bool StreamTranscoder::canSwitchToGenerator()
{
    if(_sourceBuffer && _frameBuffer && _generator && _outputEncoder && _transform)
        return true;
    return false;
}

void StreamTranscoder::needToSwitchToGenerator(const bool needToSwitch)
{
    if(needToSwitch && !canSwitchToGenerator())
    {
        std::stringstream os;
        LOG_WARN("The stream " << _inputStream->getStreamIndex() << " has a duration of " << getDuration()
                               << "s. It needs to switch to a generator during the process, but it cannot. "
                               << "No generator will be used for this stream.")
        return;
    }
    _needToSwitchToGenerator = needToSwitch;
}

void StreamTranscoder::setOffset(const float offset)
{
    _offset = offset;
    if(_offset > 0)
        needToSwitchToGenerator();
}

StreamTranscoder::EProcessCase StreamTranscoder::getProcessCase() const
{
    if(_inputStream && _inputDecoder && _currentDecoder == _inputDecoder)
        return eProcessCaseTranscode;
    else if(_inputStream && !_inputDecoder && !_currentDecoder)
        return eProcessCaseRewrap;
    else
        return eProcessCaseGenerator;
}
}
