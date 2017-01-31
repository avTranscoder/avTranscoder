#include "AudioReader.hpp"

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/decoder/AudioDecoder.hpp>
#include <AvTranscoder/decoder/AudioGenerator.hpp>
#include <AvTranscoder/data/decoded/AudioFrame.hpp>
#include <AvTranscoder/transform/AudioTransform.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>

namespace avtranscoder
{

AudioReader::AudioReader(const InputStreamDesc& inputDesc)
    : IReader(inputDesc)
    , _audioStreamProperties(NULL)
    , _outputSampleRate(0)
    , _outputNbChannels(0)
    , _outputSampleFormat(AV_SAMPLE_FMT_S16)
{
    init();
}

void AudioReader::init()
{
    // analyse InputFile
    avtranscoder::NoDisplayProgress p;
    _inputFile->analyse(p);
    _streamProperties = &_inputFile->getProperties().getStreamPropertiesWithIndex(_inputDesc._streamIndex);
    _audioStreamProperties = static_cast<const AudioProperties*>(_streamProperties);
    _inputFile->activateStream(_inputDesc._streamIndex);

    // setup decoder
    _decoder = new AudioDecoder(_inputFile->getStream(_inputDesc._streamIndex));
    _decoder->setupDecoder();
    _currentDecoder = _decoder;

    // create src frame
    AudioFrameDesc srcFrameDesc = _inputFile->getStream(_inputDesc._streamIndex).getAudioCodec().getAudioFrameDesc();
    if(! _inputDesc._channelIndexArray.empty())
        srcFrameDesc._nbChannels = _inputDesc._channelIndexArray.size();
    _srcFrame = new AudioFrame(srcFrameDesc, false);
    AudioFrame* srcFrame = static_cast<AudioFrame*>(_srcFrame);
    // create dst frame
    _outputSampleRate = srcFrame->getSampleRate();
    _outputNbChannels = srcFrame->getNbChannels();
    _dstFrame = new AudioFrame(AudioFrameDesc(_outputSampleRate, _outputNbChannels, getSampleFormatName(_outputSampleFormat)));

    // generator
    _generator = new AudioGenerator(srcFrameDesc);

    // create transform
    _transform = new AudioTransform();
}

AudioReader::~AudioReader()
{
    delete _decoder;
    delete _generator;
    delete _srcFrame;
    delete _dstFrame;
    delete _transform;
}

void AudioReader::updateOutput(const size_t sampleRate, const size_t nbChannels, const std::string& sampleFormat)
{
    _outputSampleRate = sampleRate;
    _outputNbChannels = nbChannels;
    _outputSampleFormat = getAVSampleFormat(sampleFormat);
    // update dst frame
    delete _dstFrame;
    _dstFrame = new AudioFrame(AudioFrameDesc(_outputSampleRate, _outputNbChannels, getSampleFormatName(_outputSampleFormat)));
}
}
