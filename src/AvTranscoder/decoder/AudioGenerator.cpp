#include "AudioGenerator.hpp"

namespace avtranscoder
{

AudioGenerator::AudioGenerator()
    : _inputFrame(NULL)
    , _silent(NULL)
{
}

AudioGenerator::AudioGenerator(const AudioGenerator& audioGenerator)
    : _inputFrame(NULL)
    , _silent(NULL)
{
}

AudioGenerator& AudioGenerator::operator=(const AudioGenerator& audioGenerator)
{
    _inputFrame = NULL;
    _silent = NULL;
    return *this;
}

AudioGenerator::~AudioGenerator()
{
    delete _silent;
}

void AudioGenerator::setNextFrame(Frame& inputFrame)
{
    _inputFrame = &inputFrame;
}

bool AudioGenerator::decodeNextFrame(Frame& frameBuffer)
{
    // Generate silent
    if(!_inputFrame)
    {
        // Generate the silent only once
        if(!_silent)
        {
            AudioFrame& audioBuffer = static_cast<AudioFrame&>(frameBuffer);
            _silent = new AudioFrame(audioBuffer.desc());
        }
        frameBuffer.getAVFrame().nb_samples = _silent->getAVFrame().nb_samples;
        frameBuffer.copyData(*_silent);
    }
    // Take audio frame from _inputFrame
    else
    {
        frameBuffer.copyData(*_inputFrame);
    }
    return true;
}

bool AudioGenerator::decodeNextFrame(Frame& frameBuffer, const size_t subStreamIndex)
{
    return decodeNextFrame(frameBuffer);
}
}
