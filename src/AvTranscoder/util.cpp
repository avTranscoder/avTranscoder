#include "util.hpp"

extern "C" {
#include <libavutil/pixdesc.h>
}

#include <utility>
#include <algorithm>

namespace avtranscoder
{

std::vector<std::string> getSupportedPixelFormats(const std::string& videoCodecName)
{
    std::vector<std::string> pixelFormats;

    // all video codec concerned
    if(videoCodecName == "")
    {
        const AVPixFmtDescriptor* pixFmtDesc = NULL;

#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(51, 44, 0)
        for(int pix_fmt = 0; pix_fmt < PIX_FMT_NB; ++pix_fmt)
            pixFmtDesc = &av_pix_fmt_descriptors[pix_fmt];
#else
        while((pixFmtDesc = av_pix_fmt_desc_next(pixFmtDesc)) != NULL)
#endif
        {
            if(!pixFmtDesc->name)
                continue;
            pixelFormats.push_back(std::string(pixFmtDesc->name));
        }
    }
    // specific video codec
    else
    {
        const AVCodec* videoCodec = avcodec_find_encoder_by_name(videoCodecName.c_str());
        if(videoCodec && videoCodec->pix_fmts != NULL)
        {
            size_t pix_fmt = 0;
            while(videoCodec->pix_fmts[pix_fmt] != -1)
            {
#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(51, 44, 0)
                const AVPixFmtDescriptor* pix_desc = &av_pix_fmt_descriptors[videoCodec->pix_fmts[pix_fmt]];
#else
                const AVPixFmtDescriptor* pix_desc = av_pix_fmt_desc_get(videoCodec->pix_fmts[pix_fmt]);
#endif
                if(!pix_desc || !pix_desc->name)
                    continue;
                pixelFormats.push_back(std::string(pix_desc->name));
                ++pix_fmt;
            }
        }
    }
    return pixelFormats;
}

std::vector<std::string> getSupportedSampleFormats(const std::string& audioCodecName)
{
    std::vector<std::string> sampleFormats;

    // all audio codec concerned
    if(audioCodecName.empty())
    {
        for(size_t sampleFormat = 0; sampleFormat < AV_SAMPLE_FMT_NB; ++sampleFormat)
        {
            sampleFormats.push_back(av_get_sample_fmt_name(static_cast<AVSampleFormat>(sampleFormat)));
        }
    }
    // specific audio codec
    else
    {
        const AVCodec* audioCodec = avcodec_find_encoder_by_name(audioCodecName.c_str());
        if(audioCodec && audioCodec->sample_fmts != NULL)
        {
            size_t sample_fmt = 0;
            while(audioCodec->sample_fmts[sample_fmt] != -1)
            {
                const char* sampleFormatName = av_get_sample_fmt_name(audioCodec->sample_fmts[sample_fmt]);
                if(sampleFormatName)
                    sampleFormats.push_back(std::string(sampleFormatName));
                sample_fmt++;
            }
        }
    }

    return sampleFormats;
}

AVPixelFormat getAVPixelFormat(const std::string& pixelFormat)
{
    return av_get_pix_fmt(pixelFormat.c_str());
}

AVSampleFormat getAVSampleFormat(const std::string& sampleFormat)
{
    return av_get_sample_fmt(sampleFormat.c_str());
}

std::string getPixelFormatName(const AVPixelFormat pixelFormat)
{
    const char* formatName = av_get_pix_fmt_name(pixelFormat);
    return formatName ? std::string(formatName) : "";
}

std::string getSampleFormatName(const AVSampleFormat sampleFormat)
{
    const char* formatName = av_get_sample_fmt_name(sampleFormat);
    return formatName ? std::string(formatName) : "";
}

std::vector<AVOutputFormat*> getAvailableFormats()
{
    std::vector<AVOutputFormat*> formats;

    AVOutputFormat* fmt = NULL;
    while((fmt = av_oformat_next(fmt)))
    {
        if(!fmt->name)
            continue;

        formats.push_back(fmt);
    }
    return formats;
}

NamesMap getAvailableFormatsNames()
{
    NamesMap formatsNames;
    std::vector<AVOutputFormat*> formats = getAvailableFormats();
    for(size_t i = 0; i < formats.size(); ++i)
    {
        AVOutputFormat* fmt = formats.at(i);
        formatsNames.insert(std::make_pair(std::string(fmt->name), std::string(fmt->long_name ? fmt->long_name : "")));
    }
    return formatsNames;
}

NamesMap getAvailableVideoFormatsNames()
{
    NamesMap formatsNames;
    std::vector<AVOutputFormat*> formats = getAvailableFormats();
    for(size_t i = 0; i < formats.size(); ++i)
    {
        AVOutputFormat* fmt = formats.at(i);
        // skip format which cannot handle video
        if(fmt->video_codec == AV_CODEC_ID_NONE)
            continue;
        formatsNames.insert(std::make_pair(std::string(fmt->name), std::string(fmt->long_name ? fmt->long_name : "")));
    }
    return formatsNames;
}

NamesMap getAvailableAudioFormatsNames()
{
    NamesMap formatsNames;
    std::vector<AVOutputFormat*> formats = getAvailableFormats();
    for(size_t i = 0; i < formats.size(); ++i)
    {
        AVOutputFormat* fmt = formats.at(i);
        // skip format which cannot handle audio
        if(fmt->audio_codec == AV_CODEC_ID_NONE)
            continue;
        formatsNames.insert(std::make_pair(std::string(fmt->name), std::string(fmt->long_name ? fmt->long_name : "")));
    }
    return formatsNames;
}

std::vector<AVCodec*> getAvailableCodecs()
{
    std::vector<AVCodec*> codecs;

    AVCodec* c = NULL;
    while((c = av_codec_next(c)))
    {
        if(!c->name)
            continue;

        codecs.push_back(c);
    }
    return codecs;
}

NamesMap getAvailableVideoCodecsNames()
{
    NamesMap videoCodecsNames;
    std::vector<AVCodec*> codecs = getAvailableCodecs();
    for(size_t i = 0; i < codecs.size(); ++i)
    {
        AVCodec* c = codecs.at(i);
        if(c->type == AVMEDIA_TYPE_VIDEO)
        {
            videoCodecsNames.insert(std::make_pair(std::string(c->name), std::string(c->long_name ? c->long_name : "")));
        }
    }
    return videoCodecsNames;
}

NamesMap getAvailableAudioCodecsNames()
{
    NamesMap audioCodecsNames;
    std::vector<AVCodec*> codecs = getAvailableCodecs();
    for(size_t i = 0; i < codecs.size(); ++i)
    {
        AVCodec* c = codecs.at(i);
        if(c->type == AVMEDIA_TYPE_AUDIO)
        {
            audioCodecsNames.insert(std::make_pair(std::string(c->name), std::string(c->long_name ? c->long_name : "")));
        }
    }
    return audioCodecsNames;
}

OptionArrayMap getAvailableOptionsPerOutputFormat()
{
    OptionArrayMap optionsPerFormat;

    AVOutputFormat* outputFormat = av_oformat_next(NULL);

    // iterate on formats
    while(outputFormat)
    {
        if(!outputFormat->name)
            continue;

        const std::string outputFormatName(outputFormat->name);
        OptionArray options;
        if(outputFormat->priv_class)
        {
            loadOptions(options, (void*)&outputFormat->priv_class, 0);
        }
        optionsPerFormat.insert(std::make_pair(outputFormatName, options));
        outputFormat = av_oformat_next(outputFormat);
    }
    return optionsPerFormat;
}

OptionArrayMap getAvailableOptionsPerVideoCodec()
{
    OptionArrayMap videoCodecOptions;

    AVCodec* codec = av_codec_next(NULL);

    // iterate on codecs
    while(codec)
    {
        if(!codec->name)
            continue;

        // add only video codec
        if(codec->type == AVMEDIA_TYPE_VIDEO)
        {
            const std::string videoCodecName(codec->name);
            OptionArray options;
            if(codec->priv_class)
            {
                loadOptions(options, (void*)&codec->priv_class, 0);
            }
            videoCodecOptions.insert(std::make_pair(videoCodecName, options));
        }
        codec = av_codec_next(codec);
    }
    return videoCodecOptions;
}

OptionArrayMap getAvailableOptionsPerAudioCodec()
{
    OptionArrayMap audioCodecOptions;

    AVCodec* codec = av_codec_next(NULL);

    // iterate on codecs
    while(codec)
    {
        if(!codec->name)
            continue;

        // add only audio codec
        if(codec->type == AVMEDIA_TYPE_AUDIO)
        {
            const std::string audioCodecName(codec->name);
            OptionArray options;
            if(codec->priv_class)
            {
                loadOptions(options, (void*)&codec->priv_class, 0);
            }
            audioCodecOptions.insert(std::make_pair(audioCodecName, options));
        }
        codec = av_codec_next(codec);
    }
    return audioCodecOptions;
}
}
