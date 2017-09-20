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
#include <algorithm>

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder(IInputStream& inputStream, IOutputFile& outputFile, const float offset)
    : _inputStreamDesc()
    , _inputStreams()
    , _outputStream(NULL)
    , _decodedData()
    , _filteredData(NULL)
    , _transformedData(NULL)
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
                _decodedData.push_back(new VideoFrame(inputFrameDesc));
                _filteredData = new VideoFrame(inputFrameDesc);
                _transformedData = new VideoFrame(inputFrameDesc);

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
                _decodedData.push_back(new AudioFrame(inputFrameDesc));
                _filteredData = new AudioFrame(inputFrameDesc);
                _transformedData = new AudioFrame(inputFrameDesc);

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

StreamTranscoder::StreamTranscoder(const std::vector<InputStreamDesc>& inputStreamsDesc, std::vector<IInputStream*>& inputStreams, IOutputFile& outputFile, 
                                   const ProfileLoader::Profile& profile, const float offset)
    : _inputStreamDesc(inputStreamsDesc)
    , _inputStreams(inputStreams)
    , _outputStream(NULL)
    , _decodedData()
    , _filteredData(NULL)
    , _transformedData(NULL)
    , _inputDecoders()
    , _generators()
    , _currentDecoder(NULL)
    , _outputEncoder(NULL)
    , _transform(NULL)
    , _filterGraph(NULL)
    , _offset(offset)
    , _needToSwitchToGenerator(false)
{
    // add as many decoders as input streams
    size_t nbOutputChannels = 0;
    for(size_t index = 0; index < inputStreams.size(); ++index)
    {
        if(_inputStreams.at(index) != NULL)
        {
            LOG_INFO("add decoder for input stream " << index);
            addDecoder(_inputStreamDesc.at(index), *_inputStreams.at(index));
            nbOutputChannels += _inputStreamDesc.at(index)._channelIndexArray.size();
        }
        else
        {
            LOG_INFO("add generator for empty input " << index);
            addGenerator(_inputStreamDesc.at(index), profile);
        }
    }

    const size_t firstInputStreamIndex = getFirstInputStreamIndex();
    IInputStream& inputStream = *_inputStreams.at(firstInputStreamIndex);
    const InputStreamDesc& inputStreamDesc = inputStreamsDesc.at(firstInputStreamIndex);

    // create a transcode case
    switch(inputStream.getProperties().getStreamType())
    {
        case AVMEDIA_TYPE_VIDEO:
        {
            // filter
            _filterGraph = new FilterGraph(inputStream.getVideoCodec());

            // output encoder
            VideoEncoder* outputVideo = new VideoEncoder(profile.at(constants::avProfileCodec));
            _outputEncoder = outputVideo;

            VideoFrameDesc outputFrameDesc = inputStream.getVideoCodec().getVideoFrameDesc();
            outputFrameDesc.setParameters(profile);
            outputVideo->setupVideoEncoder(outputFrameDesc, profile);

            // output stream
            _outputStream = &outputFile.addVideoStream(outputVideo->getVideoCodec());

            // buffers to process
            _filteredData = new VideoFrame(inputStream.getVideoCodec().getVideoFrameDesc());
            _transformedData = new VideoFrame(outputVideo->getVideoCodec().getVideoFrameDesc());

            // transform
            _transform = new VideoTransform();

            break;
        }
        case AVMEDIA_TYPE_AUDIO:
        {
            // filter
            _filterGraph = new FilterGraph(inputStream.getAudioCodec());
            // merge two or more audio streams into a single multi-channel stream.
            if(inputStreams.size() > 1)
            {
                std::stringstream mergeOptions;
                mergeOptions << "inputs=" << inputStreams.size();
                _filterGraph->addFilter("amerge", mergeOptions.str());
            }

            // output encoder
            AudioEncoder* outputAudio = new AudioEncoder(profile.at(constants::avProfileCodec));
            _outputEncoder = outputAudio;

            AudioFrameDesc outputFrameDesc(inputStream.getAudioCodec().getAudioFrameDesc());
            outputFrameDesc.setParameters(profile);
            if(inputStreamDesc.demultiplexing())
                outputFrameDesc._nbChannels = nbOutputChannels;
            outputAudio->setupAudioEncoder(outputFrameDesc, profile);

            // output stream
            _outputStream = &outputFile.addAudioStream(outputAudio->getAudioCodec());

            // buffers to process
            AudioFrameDesc inputFrameDesc(inputStream.getAudioCodec().getAudioFrameDesc());
            if(inputStreamDesc.demultiplexing())
                inputFrameDesc._nbChannels = nbOutputChannels;

            _filteredData = new AudioFrame(inputFrameDesc);
            _transformedData = new AudioFrame(outputAudio->getAudioCodec().getAudioFrameDesc());

            // transform
            _transform = new AudioTransform();

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

size_t StreamTranscoder::getFirstInputStreamIndex()
{
    for(size_t index = 0; index < _inputStreams.size(); ++index)
    {
        if(_inputStreams.at(index) != NULL)
            return index;
    }
    throw std::runtime_error("Cannot handle only null input streams");
}

void StreamTranscoder::addDecoder(const InputStreamDesc& inputStreamDesc, IInputStream& inputStream)
{
    // create a transcode case
    switch(inputStream.getProperties().getStreamType())
    {
        case AVMEDIA_TYPE_VIDEO:
        {
            // corresponding input decoder
            VideoDecoder* inputVideo = new VideoDecoder(static_cast<InputStream&>(inputStream));
            inputVideo->setupDecoder();
            _inputDecoders.push_back(inputVideo);
            _currentDecoder = inputVideo;

            // buffers to get the decoded data
            VideoFrame* inputFrame = new VideoFrame(inputStream.getVideoCodec().getVideoFrameDesc(), false);
            _decodedData.push_back(inputFrame);

            // generator decoder
            _generators.push_back(new VideoGenerator(inputStream.getVideoCodec().getVideoFrameDesc()));

            break;
        }
        case AVMEDIA_TYPE_AUDIO:
        {
            // corresponding input decoder
            AudioDecoder* inputAudio = new AudioDecoder(static_cast<InputStream&>(inputStream));
            inputAudio->setupDecoder();
            _inputDecoders.push_back(inputAudio);
            _currentDecoder = inputAudio;

            // buffers to get the decoded data
            AudioFrameDesc inputFrameDesc(inputStream.getAudioCodec().getAudioFrameDesc());
            if(inputStreamDesc.demultiplexing())
                inputFrameDesc._nbChannels = inputStreamDesc._channelIndexArray.size();
            _decodedData.push_back(new AudioFrame(inputFrameDesc, false));

            // generator decoder
            _generators.push_back(new AudioGenerator(inputFrameDesc));

            break;
        }
        default:
        {
            throw std::runtime_error("Unupported stream type");
            break;
        }
    }
}

void StreamTranscoder::addGenerator(const InputStreamDesc& inputStreamDesc, const ProfileLoader::Profile& profile)
{
    // create a transcode case
    if(profile.find(constants::avProfileType)->second == constants::avProfileTypeVideo)
    {
        VideoCodec inputVideoCodec(eCodecTypeEncoder, profile.find(constants::avProfileCodec)->second);
        VideoFrameDesc inputFrameDesc(profile);
        inputVideoCodec.setImageParameters(inputFrameDesc);

        // generator decoder
        VideoGenerator* generator = new VideoGenerator(inputFrameDesc);
        _generators.push_back(generator);
        _currentDecoder = generator;

        // buffers to process
        VideoFrameDesc outputFrameDesc = inputFrameDesc;
        outputFrameDesc.setParameters(profile);
        _decodedData.push_back(new VideoFrame(inputFrameDesc));

        // no decoder for this input
        _inputDecoders.push_back(NULL);

    }
    else if(profile.find(constants::avProfileType)->second == constants::avProfileTypeAudio)
    {
        // corresponding input codec
        AudioCodec inputAudioCodec(eCodecTypeEncoder, profile.find(constants::avProfileCodec)->second);
        AudioFrameDesc inputFrameDesc(profile);
        inputFrameDesc._nbChannels = 1;
        inputAudioCodec.setAudioParameters(inputFrameDesc);

        // generator decoder
        AudioGenerator* generator = new AudioGenerator(inputFrameDesc);
        _generators.push_back(generator);
        _currentDecoder = generator;
        // buffers to get the decoded data
        _decodedData.push_back(new AudioFrame(inputFrameDesc));

        // no decoder for this input
        _inputDecoders.push_back(NULL);
    }
    else
    {
        throw std::runtime_error("unupported stream type");
    }
}

StreamTranscoder::StreamTranscoder(IOutputFile& outputFile, const ProfileLoader::Profile& profile)
    : _inputStreamDesc()
    , _inputStreams()
    , _outputStream(NULL)
    , _decodedData()
    , _filteredData(NULL)
    , _transformedData(NULL)
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
        VideoFrameDesc inputFrameDesc(profile);
        inputVideoCodec.setImageParameters(inputFrameDesc);

        // generator decoder
        VideoGenerator* generator = new VideoGenerator(inputFrameDesc);
        _generators.push_back(generator);
        _currentDecoder = generator;

        // filter
        _filterGraph = new FilterGraph(inputVideoCodec);

        // buffers to process
        VideoFrameDesc outputFrameDesc = inputFrameDesc;
        outputFrameDesc.setParameters(profile);
        _decodedData.push_back(new VideoFrame(inputFrameDesc));
        _filteredData = new VideoFrame(inputFrameDesc);
        _transformedData = new VideoFrame(outputFrameDesc);

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
        AudioFrameDesc inputFrameDesc(profile);
        inputAudioCodec.setAudioParameters(inputFrameDesc);

        // generator decoder
        AudioGenerator* generator = new AudioGenerator(inputFrameDesc);
        _generators.push_back(generator);
        _currentDecoder = generator;

        // filter
        _filterGraph = new FilterGraph(inputAudioCodec);

        // buffers to process
        AudioFrameDesc outputFrameDesc = inputFrameDesc;
        outputFrameDesc.setParameters(profile);
        _decodedData.push_back(new AudioFrame(inputFrameDesc));
        _filteredData = new AudioFrame(inputFrameDesc);
        _transformedData = new AudioFrame(outputFrameDesc);

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
    for(std::vector<IFrame*>::iterator it = _decodedData.begin(); it != _decodedData.end(); ++it)
    {
        delete(*it);
    }
    delete _filteredData;
    delete _transformedData;

    for(std::vector<IDecoder*>::iterator it = _inputDecoders.begin(); it != _inputDecoders.end(); ++it)
    {
        if(*it != NULL)
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

            // free our frame data since some new buffers will be allocated by the decoders in the next step
            for(std::vector<IFrame*>::iterator it = _decodedData.begin(); it != _decodedData.end(); ++it)
            {
                if((*it)->isDataAllocated())
                    (*it)->freeData();
            }
            // switch the decoder
            if(! _inputDecoders.empty())
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

    // Decode
    LOG_DEBUG("Decode next frame")
    std::vector<bool> decodingStatus(_generators.size(), true);
    for(size_t index = 0; index < _generators.size(); ++index)
    {
        EProcessCase processCase = getProcessCase(index);
        if(processCase == eProcessCaseTranscode)
            _currentDecoder = _inputDecoders.at(index);
        else
            _currentDecoder = _generators.at(index);

        if(! _inputStreamDesc.empty() && _inputStreamDesc.at(index).demultiplexing())
            decodingStatus.at(index) = decodingStatus.at(index) && _currentDecoder->decodeNextFrame(*_decodedData.at(index), _inputStreamDesc.at(index)._channelIndexArray);
        else
            decodingStatus.at(index) = decodingStatus.at(index) && _currentDecoder->decodeNextFrame(*_decodedData.at(index));
    }

    // check the next data buffers in case of audio frames
    const size_t firstInputStreamIndex = getFirstInputStreamIndex();
    if(_decodedData.at(firstInputStreamIndex)->isAudioFrame())
    {
        const int nbInputSamplesPerChannel = _decodedData.at(firstInputStreamIndex)->getAVFrame().nb_samples;

        // Reallocate output frame
        if(nbInputSamplesPerChannel > _filteredData->getAVFrame().nb_samples)
        {
            LOG_WARN("The buffer of filtered data corresponds to a frame of " << _filteredData->getAVFrame().nb_samples << " samples. The decoded buffer contains " << nbInputSamplesPerChannel << " samples. Reallocate it.")
            _filteredData->freeData();
            _filteredData->getAVFrame().nb_samples = nbInputSamplesPerChannel;
            _filteredData->allocateData();
        }
        if(nbInputSamplesPerChannel > _transformedData->getAVFrame().nb_samples)
        {
            LOG_WARN("The buffer of transformed data corresponds to a frame of " << _transformedData->getAVFrame().nb_samples << " samples. The decoded buffer contains " << nbInputSamplesPerChannel << " samples. Reallocate it.")
            _transformedData->freeData();
            _transformedData->getAVFrame().nb_samples = nbInputSamplesPerChannel;
            _transformedData->allocateData();
        }
    }

    // Transform
    CodedData data;
    bool continueProcess = true;
    for(size_t index = 0; index < decodingStatus.size(); ++index)
    {
        if(!decodingStatus.at(index))
        {
            if(!_filterGraph->hasFilters() || !_filterGraph->hasBufferedFrames(index))
            {
                continueProcess = false;
                break;
            }
            LOG_DEBUG("Some frames remain into filter graph buffer " << index);

            // Reset the non-decoded data as an empty frame
            _decodedData.at(index)->freeData();
            _decodedData.at(index)->getAVFrame().format = -1;
            _decodedData.at(index)->getAVFrame().nb_samples = 0;
        }
    }

    if(continueProcess)
    {
        IFrame* dataToTransform = NULL;
        if(_filterGraph->hasFilters())
        {
            LOG_DEBUG("Filtering")
            _filterGraph->process(_decodedData, *_filteredData);
            dataToTransform = _filteredData;
        }
        else
        {
            dataToTransform = _decodedData.at(0);
        }

        LOG_DEBUG("Convert")
        _transform->convert(*dataToTransform, *_transformedData);

        LOG_DEBUG("Encode")
        _outputEncoder->encodeFrame(*_transformedData, data);
    }
    else
    {
        LOG_DEBUG("Encode last frame(s)")
        if(!_outputEncoder->encodeFrame(data))
        {
            if(_needToSwitchToGenerator)
            {
                switchToGeneratorDecoder();
                LOG_INFO("Force reallocation of the decoded data buffers since the decoders could have cleared them.")
                for(std::vector<IFrame*>::iterator it = _decodedData.begin(); it != _decodedData.end(); ++it)
                {
                    if(! (*it)->isDataAllocated())
                        (*it)->allocateData();
                }
                return processTranscode();
            }
            return false;
        }
    }

    // Wrap
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
            IInputStream* inputStream = _inputStreams.at(index);
            if(inputStream == NULL)
                continue;

            const StreamProperties& streamProperties = inputStream->getProperties();
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
    {
        needToSwitchToGenerator();
        // allocate the frame since the process will start with some generated data
        for(std::vector<IFrame*>::iterator it = _decodedData.begin(); it != _decodedData.end(); ++it)
        {
            if(! (*it)->isDataAllocated())
                (*it)->allocateData();
        }
    }
}

StreamTranscoder::EProcessCase StreamTranscoder::getProcessCase(const size_t decoderIndex) const
{
    if(_inputStreamDesc.size() <= 1)
    {
        if(! _inputStreams.empty() && ! _inputDecoders.empty() && std::find(_inputDecoders.begin(), _inputDecoders.end(), _currentDecoder) != _inputDecoders.end() )
            return eProcessCaseTranscode;
        else if(! _inputStreams.empty() && _inputDecoders.empty() && !_currentDecoder)
            return eProcessCaseRewrap;
        else
            return eProcessCaseGenerator;
    }
    else
    {
        if(! _inputStreams.empty() && _currentDecoder != NULL)
        {
            if( _inputStreams.at(decoderIndex) != NULL)
                return eProcessCaseTranscode;
            return eProcessCaseGenerator;
        }
        else if(! _inputStreams.empty() && _inputDecoders.empty() && !_currentDecoder)
        {
            return eProcessCaseRewrap;
        }
        return eProcessCaseGenerator;
    }
}
}
