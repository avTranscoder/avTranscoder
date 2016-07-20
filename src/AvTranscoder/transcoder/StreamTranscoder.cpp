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
    : _inputStreamDesc()
    , _inputStreams()
    , _outputStream(NULL)
    , _decodedData()
    , _filteredData()
    , _transformedData()
    , _inputDecoders()
    , _generators()
    , _currentDecoder(NULL)
    , _outputEncoder(NULL)
    , _transform(NULL)
    , _filterGraph(NULL)
    , _offset(offset)
    , _needToSwitchToGenerator(false)
{
    _inputStreams.push_back(&inputStream);

    // create a re-wrapping case
    switch(inputStream.getProperties().getStreamType())
    {
        case AVMEDIA_TYPE_VIDEO:
        {
            // output stream
            _outputStream = &outputFile.addVideoStream(inputStream.getVideoCodec());

            try
            {
                // filter
                _filterGraph = new FilterGraph(inputStream.getVideoCodec());

                VideoFrameDesc inputFrameDesc(inputStream.getVideoCodec().getVideoFrameDesc());

                // generator decoder
                _generators.push_back(new VideoGenerator(inputFrameDesc));

                // buffers to process
                _decodedData.push_back(VideoFrame(inputFrameDesc));
                _filteredData = VideoFrame(inputFrameDesc);
                _transformedData = VideoFrame(inputFrameDesc);

                // transform
                _transform = new VideoTransform();

                // output encoder
                VideoEncoder* outputVideo = new VideoEncoder(inputStream.getVideoCodec().getCodecName());
                outputVideo->setupVideoEncoder(inputFrameDesc);
                _outputEncoder = outputVideo;
            }
            catch(std::runtime_error& e)
            {
                LOG_WARN("Cannot create the video encoder for stream " << inputStream.getStreamIndex() << " if needed. "
                                                                       << e.what())
            }

            break;
        }
        case AVMEDIA_TYPE_AUDIO:
        {
            // output stream
            _outputStream = &outputFile.addAudioStream(inputStream.getAudioCodec());

            try
            {
                // filter
                _filterGraph = new FilterGraph(inputStream.getAudioCodec());

                AudioFrameDesc inputFrameDesc(inputStream.getAudioCodec().getAudioFrameDesc());

                // generator decoder
                _generators.push_back(new AudioGenerator(inputFrameDesc));

                // buffers to process
                _decodedData.push_back(AudioFrame(inputFrameDesc));
                _filteredData = AudioFrame(inputFrameDesc);
                _transformedData = AudioFrame(inputFrameDesc);

                // transform
                _transform = new AudioTransform();

                // output encoder
                AudioEncoder* outputAudio = new AudioEncoder(inputStream.getAudioCodec().getCodecName());
                outputAudio->setupAudioEncoder(inputFrameDesc);
                _outputEncoder = outputAudio;
            }

            catch(std::runtime_error& e)
            {
                LOG_WARN("Cannot create the audio encoder for stream " << inputStream.getStreamIndex() << " if needed. "
                                                                       << e.what())
            }

            break;
        }
        case AVMEDIA_TYPE_DATA:
        {
            // @warning: rewrap a data stream can't be lengthen by a generator (end of rewrapping will end the all process)
            _outputStream = &outputFile.addDataStream(inputStream.getDataCodec());
            break;
        }
        default:
            break;
    }
    setOffset(offset);
}

StreamTranscoder::StreamTranscoder(const InputStreamDesc& inputStreamDesc, IInputStream& inputStream, IOutputFile& outputFile,
                                   const ProfileLoader::Profile& profile, const float offset)
    : _inputStreamDesc()
    , _inputStreams()
    , _outputStream(NULL)
    , _decodedData()
    , _filteredData()
    , _transformedData()
    , _inputDecoders()
    , _generators()
    , _currentDecoder(NULL)
    , _outputEncoder(NULL)
    , _transform(NULL)
    , _filterGraph(NULL)
    , _offset(offset)
    , _needToSwitchToGenerator(false)
{
    _inputStreamDesc.push_back(inputStreamDesc);
    _inputStreams.push_back(&inputStream);

    // create a transcode case
    switch(inputStream.getProperties().getStreamType())
    {
        case AVMEDIA_TYPE_VIDEO:
        {
            // filter
            _filterGraph = new FilterGraph(inputStream.getVideoCodec());

            // input decoder
            VideoDecoder* inputVideo = new VideoDecoder(*static_cast<InputStream*>(&inputStream));
            inputVideo->setupDecoder();
            _inputDecoders.push_back(inputVideo);
            _currentDecoder = inputVideo;

            // output encoder
            VideoEncoder* outputVideo = new VideoEncoder(profile.at(constants::avProfileCodec));
            _outputEncoder = outputVideo;

            VideoFrameDesc outputFrameDesc = inputStream.getVideoCodec().getVideoFrameDesc();
            outputFrameDesc.setParameters(profile);
            outputVideo->setupVideoEncoder(outputFrameDesc, profile);

            // output stream
            _outputStream = &outputFile.addVideoStream(outputVideo->getVideoCodec());

            // buffers to process
            _decodedData.push_back(VideoFrame(inputStream.getVideoCodec().getVideoFrameDesc()));
            _filteredData = VideoFrame(inputStream.getVideoCodec().getVideoFrameDesc());
            _transformedData = VideoFrame(outputVideo->getVideoCodec().getVideoFrameDesc());

            // transform
            _transform = new VideoTransform();

            // generator decoder
            _generators.push_back(new VideoGenerator(outputVideo->getVideoCodec().getVideoFrameDesc()));

            break;
        }
        case AVMEDIA_TYPE_AUDIO:
        {
            // filter
            _filterGraph = new FilterGraph(inputStream.getAudioCodec());

            // input decoder
            AudioDecoder* inputAudio = new AudioDecoder(*static_cast<InputStream*>(&inputStream));
            inputAudio->setupDecoder();
            _inputDecoders.push_back(inputAudio);
            _currentDecoder = inputAudio;

            // output encoder
            AudioEncoder* outputAudio = new AudioEncoder(profile.at(constants::avProfileCodec));
            _outputEncoder = outputAudio;

            AudioFrameDesc outputFrameDesc(inputStream.getAudioCodec().getAudioFrameDesc());
            outputFrameDesc.setParameters(profile);
            if(inputStreamDesc.demultiplexing())
                outputFrameDesc._nbChannels = inputStreamDesc._channelIndexArray.size();
            outputAudio->setupAudioEncoder(outputFrameDesc, profile);

            // output stream
            _outputStream = &outputFile.addAudioStream(outputAudio->getAudioCodec());

            // buffers to process
            AudioFrameDesc inputFrameDesc(inputStream.getAudioCodec().getAudioFrameDesc());
            if(inputStreamDesc.demultiplexing())
                inputFrameDesc._nbChannels = inputStreamDesc._channelIndexArray.size();

            _decodedData.push_back(AudioFrame(inputFrameDesc));
            _filteredData = AudioFrame(inputFrameDesc);
            _transformedData = AudioFrame(outputAudio->getAudioCodec().getAudioFrameDesc());

            // transform
            _transform = new AudioTransform();

            // generator decoder
            _generators.push_back(new AudioGenerator(outputFrameDesc));

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

StreamTranscoder::StreamTranscoder(IOutputFile& outputFile, const ProfileLoader::Profile& profile)
    : _inputStreamDesc()
    , _inputStreams()
    , _outputStream(NULL)
    , _decodedData()
    , _filteredData()
    , _transformedData()
    , _inputDecoders()
    , _generators()
    , _currentDecoder(NULL)
    , _outputEncoder(NULL)
    , _transform(NULL)
    , _filterGraph(NULL)
    , _offset(0)
    , _needToSwitchToGenerator(false)
{
    if(profile.find(constants::avProfileType)->second == constants::avProfileTypeVideo)
    {
        VideoCodec inputVideoCodec(eCodecTypeEncoder, profile.find(constants::avProfileCodec)->second);
        VideoFrameDesc inputFrameDesc;
        inputFrameDesc.setParameters(profile);
        inputVideoCodec.setImageParameters(inputFrameDesc);

        // generator decoder
        VideoGenerator* generator = new VideoGenerator(inputVideoCodec.getVideoFrameDesc());
        _generators.push_back(generator);
        _currentDecoder = generator;

        // filter
        _filterGraph = new FilterGraph(inputVideoCodec);

        // buffers to process
        VideoFrameDesc outputFrameDesc = inputFrameDesc;
        outputFrameDesc.setParameters(profile);
        _decodedData.push_back(VideoFrame(inputFrameDesc));
        _filteredData = VideoFrame(inputFrameDesc);
        _transformedData = VideoFrame(outputFrameDesc);

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
        AudioCodec inputAudioCodec(eCodecTypeEncoder, profile.find(constants::avProfileCodec)->second);
        AudioFrameDesc inputFrameDesc;
        inputFrameDesc.setParameters(profile);
        inputAudioCodec.setAudioParameters(inputFrameDesc);

        // generator decoder
        AudioGenerator* generator = new AudioGenerator(inputAudioCodec.getAudioFrameDesc());
        _generators.push_back(generator);
        _currentDecoder = generator;

        // filter
        _filterGraph = new FilterGraph(inputAudioCodec);

        // buffers to process
        AudioFrameDesc outputFrameDesc = inputFrameDesc;
        outputFrameDesc.setParameters(profile);
        _decodedData.push_back(AudioFrame(inputFrameDesc));
        _filteredData = AudioFrame(inputFrameDesc);
        _transformedData = AudioFrame(outputFrameDesc);

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
    for(std::vector<IDecoder*>::iterator it = _inputDecoders.begin(); it != _inputDecoders.end(); ++it)
    {
        delete(*it);
    }
    for(std::vector<IDecoder*>::iterator it = _generators.begin(); it != _generators.end(); ++it)
    {
        delete(*it);
    }

    delete _outputEncoder;
    delete _transform;
    delete _filterGraph;
}

void StreamTranscoder::preProcessCodecLatency()
{
    if(!_outputEncoder)
    {
        std::stringstream msg;
        msg << "No encoder found: will not preProcessCodecLatency.";
        LOG_INFO(msg.str())
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
    std::string msg = "Current process case of the stream is a ";
    switch(getProcessCase())
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

    // Manage offset
    if(_offset > 0)
    {
        const bool endOfOffset = _outputStream->getStreamDuration() >= _offset;
        if(endOfOffset)
        {
            LOG_INFO("End of positive offset")

            if(_inputDecoders.at(0))
                switchToInputDecoder();
            else
                _currentDecoder = NULL;
            _offset = 0;
        }
        else
        {
            // process generator
            if(_currentDecoder != _generators.at(0))
                switchToGeneratorDecoder();
        }
    }
    else if(_offset < 0)
    {
        bool endOfStream = false;
        for(size_t index = 0; index < _inputStreams.size(); ++index)
        {
            endOfStream = endOfStream && _outputStream->getStreamDuration() >= (_inputStreams.at(index)->getProperties().getDuration() + _offset);
        }
        if(endOfStream)
        {
            LOG_INFO("End of negative offset")

            if(_needToSwitchToGenerator)
                switchToGeneratorDecoder();
            _offset = 0;
        }
    }

    if(getProcessCase() == eProcessCaseRewrap)
        return processRewrap();
    return processTranscode();
}

bool StreamTranscoder::processRewrap()
{
    assert(_inputStreams.size() == 1);
    assert(_outputStream != NULL);

    LOG_DEBUG("StreamTranscoder::processRewrap")

    CodedData data;
    if(! _inputStreams.at(0)->readNextPacket(data))
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

bool StreamTranscoder::processTranscode()
{
    assert(_outputStream != NULL);
    assert(_currentDecoder != NULL);
    assert(_outputEncoder != NULL);
    assert(! _decodedData.empty());
    assert(_transform != NULL);

    LOG_DEBUG("StreamTranscoder::processTranscode")

    LOG_DEBUG("Decode next frame")
    bool decodingStatus = true;
    for(size_t index = 0; index < _inputDecoders.size(); ++index)
    {
        if(! _inputStreamDesc.empty() && _inputStreamDesc.at(index).demultiplexing())
            decodingStatus = decodingStatus && _currentDecoder->decodeNextFrame(_decodedData.at(index), _inputStreamDesc.at(index)._channelIndexArray);
        else
            decodingStatus = decodingStatus && _currentDecoder->decodeNextFrame(_decodedData.at(index));
    }

    CodedData data;
    if(decodingStatus)
    {
        LOG_DEBUG("Filtering")
        _filterGraph->process(_decodedData, _filteredData);

        LOG_DEBUG("Convert")
        _transform->convert(_filteredData, _transformedData);

        LOG_DEBUG("Encode")
        _outputEncoder->encodeFrame(_transformedData, data);
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

    _currentDecoder = _generators.at(0);
    assert(_currentDecoder != NULL);
}

void StreamTranscoder::switchToInputDecoder()
{
    LOG_INFO("Switch to input decoder")

    _currentDecoder = _inputDecoders.at(0);
    assert(_currentDecoder != NULL);
}

float StreamTranscoder::getDuration() const
{
    if(! _inputStreams.empty())
    {
        float minStreamDuration = -1;
        for(size_t index = 0; index < _inputStreams.size(); ++index)
        {
            const StreamProperties& streamProperties = _inputStreams.at(index)->getProperties();
            if(minStreamDuration == -1 || streamProperties.getDuration() < minStreamDuration)
                minStreamDuration = streamProperties.getDuration();
        }
        const float totalDuration = minStreamDuration + _offset;
        if(totalDuration < 0)
        {
            LOG_WARN("Offset of " << _offset << "s applied to a stream with a duration of " << minStreamDuration
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
    if(! _decodedData.empty() && ! _generators.empty() && _outputEncoder && _transform)
        return true;
    return false;
}

void StreamTranscoder::needToSwitchToGenerator(const bool needToSwitch)
{
    if(needToSwitch && !canSwitchToGenerator())
    {
        std::stringstream os;
        os << "The stream has a duration of " << getDuration() << "s.";
        os << " It needs to switch to a generator during the process, but it cannot. ";
        throw std::runtime_error(os.str());
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
    if(! _inputStreams.empty() && ! _inputDecoders.empty() && _inputDecoders.at(0) == _currentDecoder)
        return eProcessCaseTranscode;
    else if(! _inputStreams.empty() && _inputDecoders.empty() && !_currentDecoder)
        return eProcessCaseRewrap;
    else
        return eProcessCaseGenerator;
}
}
