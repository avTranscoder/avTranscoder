#include "AudioGenerator.hpp"

#include <AvTranscoder/util.hpp>

#include <sstream>

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

            std::stringstream msg;
            msg << "Generate a silence with the following features:" << std::endl;
            msg << "sample rate = " << audioBuffer.desc()._sampleRate << std::endl;
            msg << "number of channels = " << audioBuffer.desc()._nbChannels << std::endl;
            msg << "sample format = " << getSampleFormatName(audioBuffer.desc()._sampleFormat) << std::endl;
            LOG_INFO(msg.str())

            _silent = new AudioFrame(audioBuffer.desc());
        }
        LOG_INFO("Reference the silence when decode next frame")
        frameBuffer.getAVFrame().nb_samples = _silent->getAVFrame().nb_samples;
        dynamic_cast<AudioFrame&>(frameBuffer).assign(_silent->getData()[0]);
    }
    // Take audio frame from _inputFrame
    else
    {
        LOG_INFO("Reference the audio data specified when decode next frame")
        frameBuffer.refData(*_inputFrame);
    }
    return true;
}

bool AudioGenerator::decodeNextFrame(Frame& frameBuffer, const size_t subStreamIndex)
{
    return decodeNextFrame(frameBuffer);
}
}
