#include "AudioReader.hpp"

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/decoder/AudioDecoder.hpp>
#include <AvTranscoder/decoder/AudioGenerator.hpp>
#include <AvTranscoder/data/decoded/AudioFrame.hpp>
#include <AvTranscoder/transform/AudioTransform.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>

namespace avtranscoder
{

AudioReader::AudioReader(const std::string& filename, const size_t streamIndex, const int channelIndex)
    : IReader(filename, streamIndex, channelIndex)
    , _audioStreamProperties(NULL)
    , _outputSampleRate(0)
    , _outputNbChannels(0)
    , _outputSampleFormat(AV_SAMPLE_FMT_S16)
{
    init();
}

AudioReader::AudioReader(InputFile& inputFile, const size_t streamIndex, const int channelIndex)
    : IReader(inputFile, streamIndex, channelIndex)
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
    _streamProperties = &_inputFile->getProperties().getStreamPropertiesWithIndex(_streamIndex);
    _audioStreamProperties = static_cast<const AudioProperties*>(_streamProperties);
    _inputFile->activateStream(_streamIndex);

    // setup decoder
    _decoder = new AudioDecoder(_inputFile->getStream(_streamIndex));
    _decoder->setupDecoder();
    _currentDecoder = _decoder;

    // generator
    _generator = new AudioGenerator(_inputFile->getStream(_streamIndex).getAudioCodec().getAudioFrameDesc());

    // create transform
    _transform = new AudioTransform();

    // create src frame
    _srcFrame = new AudioFrame(_inputFile->getStream(_streamIndex).getAudioCodec().getAudioFrameDesc());
    AudioFrame* srcFrame = static_cast<AudioFrame*>(_srcFrame);
    // create dst frame
    _outputSampleRate = srcFrame->getSampleRate();
    _outputNbChannels = (_channelIndex == -1) ? srcFrame->getNbChannels() : 1;
    _dstFrame = new AudioFrame(AudioFrameDesc(_outputSampleRate, _outputNbChannels, _outputSampleFormat));
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
    _dstFrame = new AudioFrame(AudioFrameDesc(_outputSampleRate, _outputNbChannels, _outputSampleFormat));
}
}
