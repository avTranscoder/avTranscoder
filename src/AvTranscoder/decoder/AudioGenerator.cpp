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

bool AudioGenerator::decodeNextFrame(Frame& frameBuffer)
{
    // check the given frame
    if(!frameBuffer.isAudioFrame())
    {
        LOG_WARN("The given frame is not a valid audio frame: allocate a new AVSample to put generated data into it.");
        frameBuffer.clear();
        static_cast<AudioFrame&>(frameBuffer).allocateAVSample(_frameDesc);
    }

    // Check channel layout of the given frame to be able to copy audio data to it.
    // @see Frame.copyData method
    if(frameBuffer.getAVFrame().channel_layout == 0)
    {
        const size_t channelLayout = av_get_default_channel_layout(frameBuffer.getAVFrame().channels);
        LOG_WARN("Channel layout en the audio frame is not set. Set it to '" << channelLayout
                                                                             << "' to be able to copy silence data.")
        av_frame_set_channel_layout(&frameBuffer.getAVFrame(), channelLayout);
    }

    // Generate silent
    if(!_inputFrame)
    {
        // Generate the silent only once
        if(!_silent)
        {
            AudioFrame& audioBuffer = static_cast<AudioFrame&>(frameBuffer);
            _silent = new AudioFrame(audioBuffer.desc());

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
        LOG_DEBUG("Copy data of the silence when decode next frame")
        frameBuffer.copyData(*_silent);
    }
    // Take audio frame from _inputFrame
    else
    {
        LOG_DEBUG("Copy data of the audio specified when decode next frame")
        frameBuffer.copyData(*_inputFrame);
    }
    return true;
}

bool AudioGenerator::decodeNextFrame(Frame& frameBuffer, const std::vector<size_t> channelIndexArray)
{
    return decodeNextFrame(frameBuffer);
}
}
