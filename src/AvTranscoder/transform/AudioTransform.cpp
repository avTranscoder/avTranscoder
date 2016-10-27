#include "AudioTransform.hpp"

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/data/decoded/AudioFrame.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>

#ifdef AVTRANSCODER_LIBAV_DEPENDENCY
#include <libavresample/avresample.h>
#define AllocResampleContext avresample_alloc_context
#define FreeResampleContext avresample_free
#define InitResampleContext avresample_open
#define SetSampleFormat av_opt_set_int
#else
#include <libswresample/swresample.h>
#define AllocResampleContext swr_alloc
#define FreeResampleContext swr_free
#define InitResampleContext swr_init
#define SetSampleFormat av_opt_set_sample_fmt
#endif

#if LIBAVCODEC_VERSION_MAJOR > 54
#include <libavutil/frame.h>
#endif
}

#include <sstream>
#include <cassert>
#include <stdexcept>

namespace avtranscoder
{

AudioTransform::AudioTransform()
    : _audioConvertContext(NULL)
    , _isInit(false)
{
}

AudioTransform::~AudioTransform()
{
    FreeResampleContext(&_audioConvertContext);
}

bool AudioTransform::init(const AudioFrame& src, const AudioFrame& dst)
{
    // Set convert context
    _audioConvertContext = AllocResampleContext();
    if(!_audioConvertContext)
    {
        throw std::runtime_error("unable to create audio convert context");
    }

    av_opt_set_int(_audioConvertContext, "in_channel_layout", src.getChannelLayout(), 0);
    av_opt_set_int(_audioConvertContext, "out_channel_layout", dst.getChannelLayout(), 0);
    av_opt_set_int(_audioConvertContext, "in_sample_rate", src.getSampleRate(), 0);
    av_opt_set_int(_audioConvertContext, "out_sample_rate", dst.getSampleRate(), 0);
    SetSampleFormat(_audioConvertContext, "in_sample_fmt", src.getSampleFormat(), 0);
    SetSampleFormat(_audioConvertContext, "out_sample_fmt", dst.getSampleFormat(), 0);

    if(InitResampleContext(_audioConvertContext) < 0)
    {
        FreeResampleContext(&_audioConvertContext);
        std::stringstream msg;
        msg << "Unable to open audio convert context:" << std::endl;
        msg << "in_channel_layout " << src.getChannelLayoutDesc() << std::endl;
        msg << "out_channel_layout " << dst.getChannelLayoutDesc() << std::endl;
        msg << "in_sample_rate " << src.getSampleRate() << std::endl;
        msg << "out_sample_rate " << dst.getSampleRate() << std::endl;
        msg << "in_sample_fmt " << src.getSampleFormat() << std::endl;
        msg << "out_sample_fmt " << dst.getSampleFormat() << std::endl;
        throw std::runtime_error(msg.str());
    }

    std::stringstream msg;
    msg << "Audio conversion from " << getSampleFormatName(src.getSampleFormat()) << " to "
        << getSampleFormatName(dst.getSampleFormat()) << std::endl;
    msg << "Source, number of channels = " << src.getNbChannels() << std::endl;
    msg << "Source, channel layout = " << src.getChannelLayoutDesc() << std::endl;
    msg << "Source, sample rate = " << src.getSampleRate() << std::endl;
    msg << "Destination, number of channels = " << dst.getNbChannels() << std::endl;
    msg << "Destination, channel layout = " << dst.getChannelLayoutDesc() << std::endl;
    msg << "Destination, sample rate = " << dst.getSampleRate() << std::endl;
    LOG_INFO(msg.str())

    return true;
}

void AudioTransform::convert(const Frame& srcFrame, Frame& dstFrame)
{
    const AudioFrame& src = static_cast<const AudioFrame&>(srcFrame);
    const AudioFrame& dst = static_cast<const AudioFrame&>(dstFrame);

    assert(src.getSampleRate() > 0);
    assert(src.getNbChannels() > 0);
    assert(src.getNbSamplesPerChannel() > 0);
    assert(src.getSampleFormat() != AV_SAMPLE_FMT_NONE);
    assert(dst.getSize() > 0);

    if(!_isInit)
        _isInit = init(src, dst);

    // if number of samples change from previous frame
    const size_t nbInputSamplesPerChannel = srcFrame.getAVFrame().nb_samples;

    const unsigned char** srcData = srcFrame.getData();
    unsigned char** dstData = dstFrame.getData();

    int nbOutputSamplesPerChannel;
#ifdef AVTRANSCODER_LIBAV_DEPENDENCY
    nbOutputSamplesPerChannel =
        avresample_convert(_audioConvertContext, dstData, 0, nbInputSamplesPerChannel, srcData, 0, nbInputSamplesPerChannel);
#else
    nbOutputSamplesPerChannel =
        swr_convert(_audioConvertContext, dstData, nbInputSamplesPerChannel, srcData, nbInputSamplesPerChannel);
#endif

    // update the number of samples of the output frame
    if(nbOutputSamplesPerChannel < 0)
        throw std::runtime_error("Unable to convert audio samples");
    dstFrame.getAVFrame().nb_samples = nbOutputSamplesPerChannel;
}
}
