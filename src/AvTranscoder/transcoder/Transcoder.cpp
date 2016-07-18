#include "Transcoder.hpp"

#include <AvTranscoder/file/util.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/stat/VideoStat.hpp>

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
    , _outputDuration(0)
{
}

Transcoder::~Transcoder()
{
    for(std::vector<InputFile*>::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it)
    {
        delete(*it);
    }
    for(std::vector<StreamTranscoder*>::iterator it = _streamTranscodersAllocated.begin();
        it != _streamTranscodersAllocated.end(); ++it)
    {
        delete(*it);
    }
}

void Transcoder::add(const InputStreamDesc& inputStreamDesc,
                     const std::string& profileName, const float offset)
{
    // Check filename
    if(inputStreamDesc._filename.length() == 0)
        throw std::runtime_error("Can't process a stream without a filename indicated.");

    if(profileName.length() == 0)
    {
        // Re-wrap
        if(inputStreamDesc._channelIndex < 0)
            addRewrapStream(inputStreamDesc, offset);
        // Transcode (transparent for the user)
        else
            addTranscodeStream(inputStreamDesc, offset);
    }
    // Transcode
    else
    {
        const ProfileLoader::Profile& encodingProfile = _profileLoader.getProfile(profileName);
        add(inputStreamDesc, encodingProfile, offset);
    }
}

void Transcoder::add(const InputStreamDesc& inputStreamDesc,
                     const ProfileLoader::Profile& profile, const float offset)
{
    // Check filename
    if(!inputStreamDesc._filename.length())
        throw std::runtime_error("Can't transcode a stream without a filename indicated.");

    addTranscodeStream(inputStreamDesc, profile, offset);
}

void Transcoder::add(const std::string& profile)
{
    const ProfileLoader::Profile& encodingProfile = _profileLoader.getProfile(profile);
    add(encodingProfile);
}

void Transcoder::add(const ProfileLoader::Profile& profile)
{
    addGeneratedStream(profile);
}

void Transcoder::add(StreamTranscoder& stream)
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
    if(_streamTranscoders.size() == 0)
        return false;

    // For each stream, process a frame
    size_t nbStreamProcessStatusFailed = 0;
    for(size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex)
    {
        LOG_DEBUG("Process stream " << streamIndex << "/" << (_streamTranscoders.size() - 1))
        if(!_streamTranscoders.at(streamIndex)->processFrame())
        {
            LOG_WARN("Failed to process stream " << streamIndex)
            ++nbStreamProcessStatusFailed;
        }
    }

    // Get the number of streams without the generators (they always succeed)
    size_t nbStreamsWithoutGenerator = _streamTranscoders.size();
    for(size_t streamIndex = 0; streamIndex < _streamTranscoders.size(); ++streamIndex)
    {
        if(_streamTranscoders.at(streamIndex)->getProcessCase() == StreamTranscoder::eProcessCaseGenerator)
            --nbStreamsWithoutGenerator;
    }

    // If all streams failed to process a new frame
    if(nbStreamsWithoutGenerator != 0 && nbStreamsWithoutGenerator == nbStreamProcessStatusFailed)
    {
        LOG_INFO("End of process because all streams (except generators) failed to process a new frame.")
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

    size_t frame = 0;
    bool frameProcessed = true;
    while(frameProcessed)
    {
        LOG_DEBUG("Process frame " << frame)
        frameProcessed = processFrame();
        ++frame;

        const float progressDuration = getCurrentOutputDuration();

        // check if JobStatusCancel
        if(progress.progress((progressDuration > expectedOutputDuration) ? expectedOutputDuration : progressDuration,
                             expectedOutputDuration) == eJobStatusCancel)
        {
            LOG_INFO("End of process because the job was canceled.")
            break;
        }

        // check progressDuration
        if(progressDuration >= expectedOutputDuration)
        {
            LOG_INFO("End of process because the output program duration ("
                     << progressDuration << "s) is equal or upper than " << expectedOutputDuration << "s.")
            break;
        }
    }

    _outputFile.endWrap();

    LOG_INFO("End of process: " << frame << " frames processed")

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
    LOG_INFO("Add rewrap stream from file '" << inputStreamDesc._filename << "' / index=" << inputStreamDesc._streamIndex << " / offset=" << offset << "s")

    InputFile* referenceFile = addInputFile(inputStreamDesc._filename, inputStreamDesc._streamIndex, offset);

    _streamTranscodersAllocated.push_back(new StreamTranscoder(referenceFile->getStream(inputStreamDesc._streamIndex), _outputFile, offset));
    _streamTranscoders.push_back(_streamTranscodersAllocated.back());
}

void Transcoder::addTranscodeStream(const InputStreamDesc& inputStreamDesc,
                                    const float offset)
{
    // Get profile from input file
    InputFile inputFile(inputStreamDesc._filename);
    ProfileLoader::Profile profile = getProfileFromFile(inputFile, inputStreamDesc._streamIndex);

    // override channels parameter to manage demultiplexing
    ProfileLoader::Profile::iterator it = profile.find(constants::avProfileChannel);
    if(it != profile.end())
        it->second = "1";

    addTranscodeStream(inputStreamDesc, profile, offset);
}

void Transcoder::addTranscodeStream(const InputStreamDesc& inputStreamDesc,
                                    const ProfileLoader::Profile& profile, const float offset)
{
    // Add profile
    if(!_profileLoader.hasProfile(profile))
        _profileLoader.loadProfile(profile);

    LOG_INFO("Add transcode stream from file '"
             << inputStreamDesc._filename << "' / index=" << inputStreamDesc._streamIndex << " / channel=" << inputStreamDesc._channelIndex
             << " / encodingProfile=" << profile.at(constants::avProfileIdentificatorHuman) << " / offset=" << offset << "s")

    // Add input file
    InputFile* referenceFile = addInputFile(inputStreamDesc._filename, inputStreamDesc._streamIndex, offset);

    switch(referenceFile->getStream(inputStreamDesc._streamIndex).getProperties().getStreamType())
    {
        case AVMEDIA_TYPE_VIDEO:
        case AVMEDIA_TYPE_AUDIO:
        {
            _streamTranscodersAllocated.push_back(
                new StreamTranscoder(referenceFile->getStream(inputStreamDesc._streamIndex), _outputFile, profile, inputStreamDesc._channelIndex, offset));
            _streamTranscoders.push_back(_streamTranscodersAllocated.back());
            break;
        }
        case AVMEDIA_TYPE_DATA:
        case AVMEDIA_TYPE_SUBTITLE:
        case AVMEDIA_TYPE_ATTACHMENT:
        default:
        {
            throw std::runtime_error("unsupported media type in transcode setup");
        }
    }
}

void Transcoder::addGeneratedStream(const ProfileLoader::Profile& profile)
{
    // Add profile
    if(!_profileLoader.hasProfile(profile))
        _profileLoader.loadProfile(profile);

    LOG_INFO("Add generated stream with encodingProfile=" << profile.at(constants::avProfileIdentificatorHuman))

    _streamTranscodersAllocated.push_back(new StreamTranscoder(_outputFile, profile));
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

ProfileLoader::Profile Transcoder::getProfileFromFile(InputFile& inputFile, const size_t streamIndex)
{
    const StreamProperties* streamProperties = &inputFile.getProperties().getStreamPropertiesWithIndex(streamIndex);
    const VideoProperties* videoProperties = NULL;
    const AudioProperties* audioProperties = NULL;
    switch(inputFile.getStream(streamIndex).getProperties().getStreamType())
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
        profile[constants::avProfileWidth] = videoProperties->getWidth();
        profile[constants::avProfileHeight] = videoProperties->getHeight();
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
        ss << audioProperties->getNbChannels();
        profile[constants::avProfileChannel] = ss.str();
    }

    return profile;
}

float Transcoder::getStreamDuration(size_t indexStream) const
{
    return _streamTranscoders.at(indexStream)->getDuration();
}

float Transcoder::getMinTotalDuration() const
{
    float minTotalDuration = std::numeric_limits<float>::max();
    for(size_t i = 0; i < _streamTranscoders.size(); ++i)
    {
        minTotalDuration = std::min(getStreamDuration(i), minTotalDuration);
    }
    return minTotalDuration;
}

float Transcoder::getMaxTotalDuration() const
{
    float maxTotalDuration = 0;
    for(size_t i = 0; i < _streamTranscoders.size(); ++i)
    {
        maxTotalDuration = std::max(getStreamDuration(i), maxTotalDuration);
    }
    return maxTotalDuration;
}

float Transcoder::getExpectedOutputDuration() const
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
        const IInputStream* inputStream = _streamTranscoders.at(streamIndex)->getInputStream();
        if(inputStream == NULL)
        {
            LOG_WARN("Cannot process statistics of generated stream.")
            continue;
        }
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
                    if(encoderContext.coded_frame && (encoderContext.flags & CODEC_FLAG_PSNR))
                    {
                        videoStat.setQuality(encoderContext.coded_frame->quality);
                        videoStat.setPSNR(encoderContext.coded_frame->error[0] /
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
