#include "Transcoder.hpp"

#include <AvTranscoder/file/util.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/stat/VideoStat.hpp>

#include <cassert>
#include <limits>
#include <algorithm>

namespace avtranscoder
{

Transcoder::Transcoder(IOutputFile& outputFile)
    : _outputFile(outputFile)
    , _inputFiles()
    , _streamTranscoders()
    , _streamTranscodersAllocated()
    , _profileLoader(true)
    , _eProcessMethod(eProcessMethodLongest)
    , _mainStreamIndex(0)
    , _processedFrames(0)
    , _outputDuration(0)
{
}

Transcoder::~Transcoder()
{
    for(std::vector<StreamTranscoder*>::iterator it = _streamTranscodersAllocated.begin();
        it != _streamTranscodersAllocated.end(); ++it)
    {
        delete(*it);
    }

    for(std::vector<InputFile*>::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it)
    {
        delete(*it);
    }
}

void Transcoder::addStream(const InputStreamDesc& inputStreamDesc, const std::string& profileName, const float offset)
{
    // Check filename
    if(inputStreamDesc._filename.empty())
        throw std::runtime_error("Can't process a stream without a filename indicated.");

    if(profileName.empty())
    {
        // Re-wrap
        if(!inputStreamDesc.demultiplexing())
            addRewrapStream(inputStreamDesc, offset);
        // Transcode (transparent for the user)
        else
        {
            const ProfileLoader::Profile profile = getProfileFromInput(inputStreamDesc);
            addStream(inputStreamDesc, profile, offset);
        }
    }
    // Transcode
    else
    {
        const ProfileLoader::Profile& encodingProfile = _profileLoader.getProfile(profileName);
        addStream(inputStreamDesc, encodingProfile, offset);
    }
}

void Transcoder::addStream(const InputStreamDesc& inputStreamDesc, const ProfileLoader::Profile& profile, const float offset)
{
    // Check filename
    if(!inputStreamDesc._filename.length())
        throw std::runtime_error("Can't transcode a stream without a filename indicated.");

    std::vector<InputStreamDesc> inputStreamDescArray;
    inputStreamDescArray.push_back(inputStreamDesc);
    addStream(inputStreamDescArray, profile, offset);
}

void Transcoder::addStream(const InputStreamDesc& inputStreamDesc, IEncoder* encoder)
{
    // Check filename
    if(!inputStreamDesc._filename.length())
        throw std::runtime_error("Can't transcode a stream without a filename indicated.");

    std::vector<InputStreamDesc> inputStreamDescArray;
    inputStreamDescArray.push_back(inputStreamDesc);
    addStream(inputStreamDescArray, encoder);
}

void Transcoder::addStream(const std::vector<InputStreamDesc>& inputStreamDescArray, const std::string& profileName, const float offset)
{
    // Check number of inputs
    if(inputStreamDescArray.empty())
        throw std::runtime_error("Need a description of at least one input stream to start the process.");

    // If there is one input, switch to an easier case
    if(inputStreamDescArray.size() == 1)
    {
        addStream(inputStreamDescArray.at(0), profileName, offset);
        return;
    }

    // Get encoding profile
    ProfileLoader::Profile encodingProfile;
    if(profileName.empty())
        encodingProfile = getProfileFromInputs(inputStreamDescArray);
    else
        encodingProfile = _profileLoader.getProfile(profileName);

    addStream(inputStreamDescArray, encodingProfile, offset);
}

void Transcoder::addStream(const std::vector<InputStreamDesc>& inputStreamDescArray, const ProfileLoader::Profile& profile, const float offset)
{
    // Check number of inputs
    if(inputStreamDescArray.empty())
        throw std::runtime_error("Need a description of at least one input stream to start the process.");

    addTranscodeStream(inputStreamDescArray, profile, offset);
}

void Transcoder::addStream(const std::vector<InputStreamDesc>& inputStreamDescArray, IEncoder* encoder)
{
    // Check number of inputs
    if(inputStreamDescArray.empty())
        throw std::runtime_error("Need a description of at least one input stream to start the process.");

    addTranscodeStream(inputStreamDescArray, encoder);
}

void Transcoder::addGenerateStream(const std::string& encodingProfileName)
{
    const ProfileLoader::Profile& encodingProfile = _profileLoader.getProfile(encodingProfileName);
    addGenerateStream(encodingProfile);
}

void Transcoder::addGenerateStream(const ProfileLoader::Profile& encodingProfile)
{
    // Add profile
    if(!_profileLoader.hasProfile(encodingProfile))
        _profileLoader.addProfile(encodingProfile);

    LOG_INFO("Add generated stream with encodingProfile=" << encodingProfile.at(constants::avProfileIdentificatorHuman))

    _streamTranscodersAllocated.push_back(new StreamTranscoder(_outputFile, encodingProfile));
    _streamTranscoders.push_back(_streamTranscodersAllocated.back());
}

void Transcoder::addStream(StreamTranscoder& stream)
{
    _streamTranscoders.push_back(&stream);
}

void Transcoder::preProcessCodecLatency()
{
    for(size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex)
    {
        LOG_DEBUG("Init stream " << streamIndex)
        _streamTranscoders.at(streamIndex)->preProcessCodecLatency();
    }
}

bool Transcoder::processFrame()
{
    NoDisplayProgress progress;
    return processFrame(progress);
}

bool Transcoder::processFrame(IProgress& progress)
{
    if(_streamTranscoders.size() == 0)
        return false;

    // For each stream, process a frame
    bool result = true;
    for(size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex)
    {
        if(!processFrame(progress, streamIndex))
            result = false;
    }
    return result;
}

bool Transcoder::processFrame(IProgress& progress, const size_t& streamIndex)
{
    LOG_DEBUG("Process stream " << streamIndex + 1 << "/" << _streamTranscoders.size())

    IOutputStream::EWrappingStatus status = _streamTranscoders.at(streamIndex)->processFrame();
    switch(status)
    {
        case IOutputStream::eWrappingSuccess:
            if(streamIndex == 0)
                _processedFrames++;

            if(!continueProcess(progress))
                return false;
            return true;

        case IOutputStream::eWrappingWaitingForData:
            // the wrapper needs more data to write the current packet
            if(streamIndex == 0)
                _processedFrames++;

            if(!continueProcess(progress))
                return false;

            return processFrame(progress, streamIndex);

        case IOutputStream::eWrappingSkip:
            return true;

        case IOutputStream::eWrappingError:
            // if a stream failed to process
            LOG_WARN("Failed to process the stream transcoder at index " << streamIndex)

            // if this is the end of the main stream
            if(streamIndex == _mainStreamIndex)
                LOG_INFO("End of process because the main stream at index " << _mainStreamIndex << " failed to process a new frame.")

            return false;
        default:
            throw std::runtime_error("Unsupported wrapping status");
    }
}

bool Transcoder::continueProcess(IProgress& progress) {
    const float expectedOutputDuration = getExpectedOutputDuration();
    const float progressDuration = getCurrentOutputDuration();

    // check if JobStatusCancel
    if(progress.progress((progressDuration > expectedOutputDuration) ? expectedOutputDuration : progressDuration,
                         expectedOutputDuration) == eJobStatusCancel)
    {
        LOG_INFO("End of process because the job was canceled.")
        return false;
    }

    // check progressDuration
    if(_eProcessMethod == eProcessMethodBasedOnDuration && progressDuration >= expectedOutputDuration)
    {
        LOG_INFO("End of process because the output program duration ("
                 << progressDuration << "s) is equal or upper than " << expectedOutputDuration << "s.")
        return false;
    }

    return true;
}

ProcessStat Transcoder::process()
{
    NoDisplayProgress progress;
    return process(progress);
}

ProcessStat Transcoder::process(IProgress& progress)
{
    if(_streamTranscoders.size() == 0)
        throw std::runtime_error("Missing input streams in transcoder");

    manageSwitchToGenerator();

    LOG_INFO("Start process")

    _outputFile.beginWrap();

    preProcessCodecLatency();

    const float expectedOutputDuration = getExpectedOutputDuration();
    LOG_INFO("The expected output duration of the program will be " << expectedOutputDuration << "s.")

    bool frameProcessed = true;
    while(frameProcessed)
    {
        LOG_INFO("Process frame " << _processedFrames);
        frameProcessed = processFrame(progress);
    }

    _outputFile.endWrap();
    LOG_INFO("End of process: " << ++_processedFrames << " frames processed")

    LOG_INFO("Get process statistics")
    ProcessStat processStat;
    fillProcessStat(processStat);

    return processStat;
}

void Transcoder::setProcessMethod(const EProcessMethod eProcessMethod, const size_t indexBasedStream,
                                  const double outputDuration)
{
    _eProcessMethod = eProcessMethod;
    _mainStreamIndex = indexBasedStream;
    _outputDuration = outputDuration;
}

void Transcoder::addRewrapStream(const InputStreamDesc& inputStreamDesc, const float offset)
{
    LOG_INFO("Add rewrap stream from " << inputStreamDesc << " with offset=" << offset << "s")

    InputFile* referenceFile = addInputFile(inputStreamDesc._filename, inputStreamDesc._streamIndex, offset);

    _streamTranscodersAllocated.push_back(
        new StreamTranscoder(referenceFile->getStream(inputStreamDesc._streamIndex), _outputFile, offset));
    _streamTranscoders.push_back(_streamTranscodersAllocated.back());
}

void Transcoder::addTranscodeStream(const std::vector<InputStreamDesc>& inputStreamDescArray, const ProfileLoader::Profile& profile,
                                    const float offset)
{
    // Add profile
    if(!_profileLoader.hasProfile(profile))
        _profileLoader.addProfile(profile);

    std::stringstream sources;
    for(size_t index = 0; index < inputStreamDescArray.size(); ++index)
        sources << inputStreamDescArray.at(index);
    LOG_INFO("Add transcode stream from the following inputs:" << std::endl << sources.str() 
                                          << "with encodingProfile=" << profile.at(constants::avProfileIdentificatorHuman) << std::endl
                                          << "and offset=" << offset << "s")

    // Create all streams from the given inputs
    std::vector<IInputStream*> inputStreams;
    AVMediaType commonStreamType = AVMEDIA_TYPE_UNKNOWN;
    for(std::vector<InputStreamDesc>::const_iterator it = inputStreamDescArray.begin(); it != inputStreamDescArray.end(); ++it)
    {
        if(it->_filename.empty())
        {
            inputStreams.push_back(NULL);
            continue;
        }

        InputFile* referenceFile = addInputFile(it->_filename, it->_streamIndex, offset);
        inputStreams.push_back(&referenceFile->getStream(it->_streamIndex));

        // Check stream type
        const AVMediaType currentStreamType = referenceFile->getProperties().getStreamPropertiesWithIndex(it->_streamIndex).getStreamType();
        if(commonStreamType == AVMEDIA_TYPE_UNKNOWN)
            commonStreamType = currentStreamType;
        else if(currentStreamType != commonStreamType)
            throw std::runtime_error("All the given inputs should be of the same type (video, audio...).");

    }

    _streamTranscodersAllocated.push_back(
                new StreamTranscoder(inputStreamDescArray, inputStreams, _outputFile, profile, offset));
    _streamTranscoders.push_back(_streamTranscodersAllocated.back());
}

void Transcoder::addTranscodeStream(const std::vector<InputStreamDesc>& inputStreamDescArray, IEncoder* encoder, const float offset)
{
    std::stringstream sources;
    for(size_t index = 0; index < inputStreamDescArray.size(); ++index)
        sources << inputStreamDescArray.at(index);
    LOG_INFO("Add transcode stream from the following inputs:" << std::endl << sources.str() 
                                          << "with encoder=" << encoder->getCodec().getCodecName() << std::endl)

    // Create all streams from the given inputs
    std::vector<IInputStream*> inputStreams;
    AVMediaType commonStreamType = AVMEDIA_TYPE_UNKNOWN;
    for(std::vector<InputStreamDesc>::const_iterator it = inputStreamDescArray.begin(); it != inputStreamDescArray.end(); ++it)
    {
        if(it->_filename.empty())
        {
            inputStreams.push_back(NULL);
            continue;
        }

        InputFile* referenceFile = addInputFile(it->_filename, it->_streamIndex, offset);
        inputStreams.push_back(&referenceFile->getStream(it->_streamIndex));

        // Check stream type
        const AVMediaType currentStreamType = referenceFile->getProperties().getStreamPropertiesWithIndex(it->_streamIndex).getStreamType();
        if(commonStreamType == AVMEDIA_TYPE_UNKNOWN)
            commonStreamType = currentStreamType;
        else if(currentStreamType != commonStreamType)
            throw std::runtime_error("All the given inputs should be of the same type (video, audio...).");

    }

    _streamTranscodersAllocated.push_back(
                new StreamTranscoder(inputStreamDescArray, inputStreams, _outputFile, encoder, offset));
    _streamTranscoders.push_back(_streamTranscodersAllocated.back());
}

InputFile* Transcoder::addInputFile(const std::string& filename, const int streamIndex, const float offset)
{
    InputFile* referenceFile = NULL;

    if(streamIndex >= 0)
    {
        for(std::vector<InputFile*>::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it)
        {
            if(((*it)->getFilename() == filename) && !(*it)->getStream(streamIndex).isActivated())
            {
                referenceFile = (*it);
                LOG_DEBUG("Get instance of InputFile from '" << filename << "'")
                break;
            }
        }
    }

    if(!referenceFile)
    {
        LOG_DEBUG("New instance of InputFile from '" << filename << "'")

        _inputFiles.push_back(new InputFile(filename));
        referenceFile = _inputFiles.back();
    }

    if(streamIndex >= 0)
        referenceFile->activateStream(streamIndex);
    else
    {
        for(size_t index = 0; index < referenceFile->getProperties().getNbStreams(); ++index)
            referenceFile->activateStream(index);
    }

    // If negative offset, move forward in the input stream
    if(offset < 0)
        referenceFile->seekAtTime(-offset);

    return referenceFile;
}

ProfileLoader::Profile Transcoder::getProfileFromInput(const InputStreamDesc& inputStreamDesc)
{
    std::vector<InputStreamDesc> inputStreamDescArray;
    inputStreamDescArray.push_back(inputStreamDesc);
    return getProfileFromInputs(inputStreamDescArray);
}

ProfileLoader::Profile Transcoder::getProfileFromInputs(const std::vector<InputStreamDesc>& inputStreamDescArray)
{
    assert(inputStreamDescArray.size() >= 1);

    // Get properties from the first input
    size_t nonEmptyFileName = std::numeric_limits<size_t>::max();
    for(size_t i = 0; i < inputStreamDescArray.size(); ++i)
    {
        if(!inputStreamDescArray.at(i)._filename.empty())
        {
            nonEmptyFileName = i;
            break;
        }
    }
    if(nonEmptyFileName == std::numeric_limits<size_t>::max())
        throw std::runtime_error("Cannot get profile from empty input streams");

    const InputStreamDesc& inputStreamDesc = inputStreamDescArray.at(nonEmptyFileName);
    InputFile inputFile(inputStreamDesc._filename);

    const StreamProperties* streamProperties = &inputFile.getProperties().getStreamPropertiesWithIndex(inputStreamDesc._streamIndex);
    const VideoProperties* videoProperties = NULL;
    const AudioProperties* audioProperties = NULL;
    switch(inputFile.getStream(inputStreamDesc._streamIndex).getProperties().getStreamType())
    {
        case AVMEDIA_TYPE_VIDEO:
        {
            videoProperties = dynamic_cast<const VideoProperties*>(streamProperties);
            break;
        }
        case AVMEDIA_TYPE_AUDIO:
        {
            audioProperties = dynamic_cast<const AudioProperties*>(streamProperties);
            break;
        }
        default:
            break;
    }

    // common fileds in profile types
    ProfileLoader::Profile profile;
    profile[constants::avProfileIdentificator] = "profileFromInput";
    profile[constants::avProfileIdentificatorHuman] = "profile from input";

    // video
    if(videoProperties != NULL)
    {
        profile[constants::avProfileType] = avtranscoder::constants::avProfileTypeVideo;
        profile[constants::avProfileCodec] = videoProperties->getCodecName();
        profile[constants::avProfilePixelFormat] = videoProperties->getPixelProperties().getPixelFormatName();
        std::stringstream ss;
        ss << videoProperties->getFps();
        profile[constants::avProfileFrameRate] = ss.str();
        ss.str(""); ss << videoProperties->getWidth();
        profile[constants::avProfileWidth] = ss.str();
        ss.str(""); ss << videoProperties->getHeight();
        profile[constants::avProfileHeight] = ss.str();
    }
    // audio
    else if(audioProperties != NULL)
    {
        profile[constants::avProfileType] = avtranscoder::constants::avProfileTypeAudio;
        profile[constants::avProfileCodec] = audioProperties->getCodecName();
        profile[constants::avProfileSampleFormat] = audioProperties->getSampleFormatName();
        std::stringstream ss;
        ss << audioProperties->getSampleRate();
        profile[constants::avProfileSampleRate] = ss.str();
        ss.str("");
        // override number of channels parameters to manage demultiplexing
        size_t nbChannels = 0;
        for(std::vector<InputStreamDesc>::const_iterator it = inputStreamDescArray.begin(); it != inputStreamDescArray.end(); ++it)
        {
            if(inputStreamDesc.demultiplexing())
                nbChannels += it->_channelIndexArray.size();
            else
            {
                InputFile inputFile(it->_filename);
                const StreamProperties& currentStream = inputFile.getProperties().getStreamPropertiesWithIndex(inputStreamDesc._streamIndex);
                if(currentStream.getStreamType() != AVMEDIA_TYPE_AUDIO)
                    throw std::runtime_error("All the given inputs should be audio streams.");
                nbChannels += dynamic_cast<const AudioProperties&>(currentStream).getNbChannels();
            }
        }
        ss << nbChannels;
        profile[constants::avProfileChannel] = ss.str();
    }

    return profile;
}

float Transcoder::getStreamDuration(const size_t indexStream) const
{
    return _streamTranscoders.at(indexStream)->getDuration();
}

float Transcoder::getMinTotalDuration()
{
    float minTotalDuration = std::numeric_limits<float>::max();
    for(size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex)
    {
        const float streamDuration = getStreamDuration(streamIndex);
        if(std::min(streamDuration, minTotalDuration) == streamDuration) 
        {
            minTotalDuration = streamDuration;
            _mainStreamIndex = streamIndex;
        }
        
    }
    return minTotalDuration;
}

float Transcoder::getMaxTotalDuration()
{
    float maxTotalDuration = 0;
    for(size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex)
    {
        const float streamDuration = getStreamDuration(streamIndex);
        if(std::max(streamDuration, maxTotalDuration) == streamDuration)
        {
            maxTotalDuration = streamDuration;
            _mainStreamIndex = streamIndex;
        }
    }
    return maxTotalDuration;
}

float Transcoder::getExpectedOutputDuration()
{
    switch(_eProcessMethod)
    {
        case eProcessMethodShortest:
            return getMinTotalDuration();
        case eProcessMethodLongest:
            return getMaxTotalDuration();
        case eProcessMethodBasedOnStream:
            return getStreamDuration(_mainStreamIndex);
        case eProcessMethodBasedOnDuration:
            return _outputDuration;
        case eProcessMethodInfinity:
            return std::numeric_limits<float>::max();
        default:
            return getMaxTotalDuration();
    }
}

float Transcoder::getCurrentOutputDuration() const
{
    float currentOutputDuration = -1;
    for(size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex)
    {
        const float currentStreamDuration = _outputFile.getStream(streamIndex).getStreamDuration();
        if(currentOutputDuration == -1)
            currentOutputDuration = currentStreamDuration;
        else if(currentStreamDuration < currentOutputDuration)
            currentOutputDuration = currentStreamDuration;
    }
    return currentOutputDuration;
}

void Transcoder::manageSwitchToGenerator()
{
    for(size_t i = 0; i < _streamTranscoders.size(); ++i)
    {
        const float currentDuration = _streamTranscoders.at(i)->getDuration();
        switch(_eProcessMethod)
        {
            case eProcessMethodShortest:
                if(_streamTranscoders.at(i)->getDuration() >= getMinTotalDuration())
                    _streamTranscoders.at(i)->needToSwitchToGenerator(false);
                else
                    _streamTranscoders.at(i)->needToSwitchToGenerator();
                break;
            case eProcessMethodLongest:
                if(_streamTranscoders.at(i)->getDuration() == getMaxTotalDuration())
                    _streamTranscoders.at(i)->needToSwitchToGenerator(false);
                else
                    _streamTranscoders.at(i)->needToSwitchToGenerator();
                break;
            case eProcessMethodBasedOnStream:
                if(i != _mainStreamIndex && currentDuration < _streamTranscoders.at(_mainStreamIndex)->getDuration())
                    _streamTranscoders.at(i)->needToSwitchToGenerator();
                else
                    _streamTranscoders.at(i)->needToSwitchToGenerator(false);
                break;
            case eProcessMethodBasedOnDuration:
                if(_streamTranscoders.at(i)->getDuration() >= _outputDuration)
                    _streamTranscoders.at(i)->needToSwitchToGenerator(false);
                else
                    _streamTranscoders.at(i)->needToSwitchToGenerator();
                break;
            case eProcessMethodInfinity:
                _streamTranscoders.at(i)->needToSwitchToGenerator();
                break;
        }
    }
}

void Transcoder::fillProcessStat(ProcessStat& processStat)
{
    for(size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex)
    {
        IOutputStream& stream = _streamTranscoders.at(streamIndex)->getOutputStream();
        if(_streamTranscoders.at(streamIndex)->getInputStreams().empty())
        {
            LOG_WARN("Cannot process statistics of generated stream.")
            continue;
        }

        size_t nonNullInputStreamIndex = 0;
        std::vector<IInputStream*> inputStreams = _streamTranscoders.at(streamIndex)->getInputStreams();
        for(size_t i = 0; i < inputStreams.size(); ++i)
        {
            if(inputStreams.at(i) != NULL)
            {
                nonNullInputStreamIndex = i;
                break;
            }
        }

        const IInputStream* inputStream = inputStreams.at(nonNullInputStreamIndex);
        const AVMediaType mediaType = inputStream->getProperties().getStreamType();
        switch(mediaType)
        {
            case AVMEDIA_TYPE_VIDEO:
            {
                VideoStat videoStat(stream.getStreamDuration(), stream.getNbFrames());
                IEncoder* encoder = _streamTranscoders.at(streamIndex)->getEncoder();
                if(encoder)
                {
                    const AVCodecContext& encoderContext = encoder->getCodec().getAVCodecContext();

#ifdef AV_CODEC_FLAG_PSNR
                    if(encoderContext.coded_side_data && encoderContext.coded_side_data->type == AV_PKT_DATA_QUALITY_FACTOR && (encoderContext.flags & AV_CODEC_FLAG_PSNR))
#else
                    if(encoderContext.coded_frame && (encoderContext.flags & CODEC_FLAG_PSNR))
#endif
                    {
                        uint8_t* coded_frame = encoderContext.coded_side_data->data;
                        uint32_t quality = (uint32_t) coded_frame[3] << 24 |
                                           (uint32_t) coded_frame[2] << 16 |
                                           (uint32_t) coded_frame[1] << 8 |
                                           (uint32_t) coded_frame[0];
                        // uint8_t picture_type = coded_frame[4];
                        uint8_t error_count = coded_frame[5];

                        std::vector<uint64_t> errors;
                        for (int i = 0; i < error_count; ++i)
                        {
                            int index = 6 + i;
                            errors.push_back(
                                (uint64_t) coded_frame[index + 7] << 56 |
                                (uint64_t) coded_frame[index + 6] << 48 |
                                (uint64_t) coded_frame[index + 5] << 40 |
                                (uint64_t) coded_frame[index + 4] << 32 |
                                (uint64_t) coded_frame[index + 3] << 24 |
                                (uint64_t) coded_frame[2] << 16 |
                                (uint64_t) coded_frame[1] << 8 |
                                (uint64_t) coded_frame[0]
                            );
                        }

                        videoStat.setQuality(quality);
                        videoStat.setPSNR((double) errors.at(0) /
                                          (encoderContext.width * encoderContext.height * 255.0 * 255.0));
                    }
                }
                processStat.addVideoStat(streamIndex, videoStat);
                break;
            }
            case AVMEDIA_TYPE_AUDIO:
            {
                AudioStat audioStat(stream.getStreamDuration(), stream.getNbFrames());
                processStat.addAudioStat(streamIndex, audioStat);
                break;
            }
            default:
                LOG_WARN("No process statistics for stream at index: " << streamIndex << " (AVMediaType = " << mediaType
                                                                       << ")")
                break;
        }
    }
}
}
