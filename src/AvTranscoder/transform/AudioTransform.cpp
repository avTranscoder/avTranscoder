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

#include <stdexcept>
#include <sstream>

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

bool AudioTransform::init(const Frame& srcFrame, const Frame& dstFrame)
{
    // Set convert context
    _audioConvertContext = AllocResampleContext();
    if(!_audioConvertContext)
    {
        throw std::runtime_error("unable to create audio convert context");
    }

    const AudioFrame& src = static_cast<const AudioFrame&>(srcFrame);
    const AudioFrame& dst = static_cast<const AudioFrame&>(dstFrame);

    av_opt_set_int(_audioConvertContext, "in_channel_layout", av_get_default_channel_layout(src.getNbChannels()), 0);
    av_opt_set_int(_audioConvertContext, "out_channel_layout", av_get_default_channel_layout(dst.getNbChannels()), 0);
    av_opt_set_int(_audioConvertContext, "in_sample_rate", src.getSampleRate(), 0);
    av_opt_set_int(_audioConvertContext, "out_sample_rate", dst.getSampleRate(), 0);
    SetSampleFormat(_audioConvertContext, "in_sample_fmt", src.getSampleFormat(), 0);
    SetSampleFormat(_audioConvertContext, "out_sample_fmt", dst.getSampleFormat(), 0);

    if(InitResampleContext(_audioConvertContext) < 0)
    {
        FreeResampleContext(&_audioConvertContext);
        std::stringstream msg;
        msg << "Unable to open audio convert context:" << std::endl;
        msg << "in_channel_layout " << av_get_default_channel_layout(src.getNbChannels()) << std::endl;
        msg << "out_channel_layout " << av_get_default_channel_layout(dst.getNbChannels()) << std::endl;
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
    msg << "Source, sample rate = " << src.getSampleRate() << std::endl;
    msg << "Destination, number of channels = " << dst.getNbChannels() << std::endl;
    msg << "Destination, sample rate = " << dst.getSampleRate() << std::endl;
    LOG_INFO(msg.str())

    return true;
}

void AudioTransform::convert(const Frame& srcFrame, Frame& dstFrame)
{
    if(!_isInit)
        _isInit = init(srcFrame, dstFrame);

    // if number of samples change from previous frame
    const size_t nbInputSamplesPerChannel = srcFrame.getAVFrame().nb_samples;

    const unsigned char** srcData = srcFrame.getData();
    unsigned char** dstData = dstFrame.getData();

    int nbOutputSamplesPerChannel;
#ifdef AVTRANSCODER_LIBAV_DEPENDENCY
    nbOutputSamplesPerChannel =
        avresample_convert(_audioConvertContext, dstData, 0, nbInputSamplesPerChannel, (uint8_t**)srcData, 0, nbInputSamplesPerChannel);
#else
    nbOutputSamplesPerChannel =
        swr_convert(_audioConvertContext, dstData, nbInputSamplesPerChannel, srcData, nbInputSamplesPerChannel);
#endif

    if(nbOutputSamplesPerChannel < 0)
    {
        throw std::runtime_error("unable to convert audio samples");
    }
    else
    {
        dstFrame.getAVFrame().nb_samples = nbOutputSamplesPerChannel;
    }
}
}
