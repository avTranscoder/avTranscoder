#include "AudioGenerator.hpp"

#include <AvTranscoder/util.hpp>

#include <sstream>

namespace avtranscoder
{

AudioGenerator::AudioGenerator(const AudioFrameDesc& frameDesc)
    : _inputFrame(NULL)
    , _silent(NULL)
    , _frameDesc(frameDesc)
{
}

AudioGenerator::~AudioGenerator()
{
    delete _silent;
}

bool AudioGenerator::decodeNextFrame(IFrame& frameBuffer)
{
    // Generate silent
    if(!_inputFrame)
    {
        // Generate the silent only once
        if(!_silent)
        {
            _silent = new AudioFrame(_frameDesc);

            std::stringstream msg;
            msg << "Generate a silence with the following features:" << std::endl;
            msg << "sample rate = " << _silent->getSampleRate() << std::endl;
            msg << "number of channels = " << _silent->getNbChannels() << std::endl;
            msg << "sample format = " << getSampleFormatName(_silent->getSampleFormat()) << std::endl;
            msg << "number of samples per channel = " << _silent->getNbSamplesPerChannel() << std::endl;
            LOG_INFO(msg.str())

            // Set the number of samples of silence to the number of samples of the given frame
            // (which was allocated to expect this number of samples).
            _silent->setNbSamplesPerChannel(frameBuffer.getAVFrame().nb_samples);
        }

        if(_silent->getNbSamplesPerChannel() != (size_t)frameBuffer.getAVFrame().nb_samples) {
            LOG_DEBUG("Reset next audio frame nb samples and reallocate.")
            frameBuffer.getAVFrame().nb_samples = _silent->getNbSamplesPerChannel();
            frameBuffer.freeData();
            frameBuffer.allocateData();
        }

        LOG_DEBUG("Copy data of the silence when decode next frame")
        frameBuffer.copyData(*_silent);
    }
    // Take audio frame from _inputFrame
    else
    {
        LOG_DEBUG("Convert data of the audio specified when decode next frame")
        _audioTransform.convert(*_inputFrame, frameBuffer);
    }

    incrementNbDecodedFrames(_silent->getNbSamplesPerChannel());
    return true;
}

bool AudioGenerator::decodeNextFrame(IFrame& frameBuffer, const std::vector<size_t> channelIndexArray)
{
    return decodeNextFrame(frameBuffer);
}
}
