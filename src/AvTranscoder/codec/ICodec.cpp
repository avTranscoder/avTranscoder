#include "ICodec.hpp"

extern "C" {
#include <libavutil/mem.h>
}

#include <stdexcept>
#include <cassert>

namespace avtranscoder
{

ICodec::ICodec(const ECodecType type, const std::string& codecName)
    : _avCodecContext(NULL)
    , _avCodec(NULL)
    , _isCodecContextAllocated(true)
    , _type(type)
{
    setCodec(type, codecName);
    allocateContext();
    loadCodecOptions();
}

ICodec::ICodec(const ECodecType type, const AVCodecID codecId)
    : _avCodecContext(NULL)
    , _avCodec(NULL)
    , _isCodecContextAllocated(true)
    , _type(type)
{
    setCodec(type, codecId);
    allocateContext();
    loadCodecOptions();
}

ICodec::ICodec(const ECodecType type, AVCodecContext& avCodecContext)
    : _avCodecContext(&avCodecContext)
    , _avCodec(NULL)
    , _isCodecContextAllocated(false)
    , _type(type)
{
    setCodec(type, _avCodecContext->codec_id);
    loadCodecOptions();
}

ICodec::~ICodec()
{
    avcodec_close(_avCodecContext);

    if(!_isCodecContextAllocated)
        return;

    avcodec_free_context(&_avCodecContext);
    _avCodecContext = NULL;
}

void ICodec::openCodec()
{
    if(!_avCodecContext)
        throw std::runtime_error("Unable to open a codec without codec context");

    const int ret = avcodec_open2(_avCodecContext, _avCodec, NULL);
    if(ret < 0)
    {
        std::string msg = "Unable to open codec: ";

        if(_avCodec && _avCodec->long_name)
            msg += _avCodec->long_name;

        if(_avCodec && _avCodec->name)
        {
            msg += " (";
            msg += _avCodec->name;
            msg += ") ";
        }

        avcodec_close(_avCodecContext);

        msg += getDescriptionFromErrorCode(ret);

        throw std::runtime_error(msg);
    }
}

std::string ICodec::getCodecName() const
{
    assert(_avCodecContext != NULL);
    const AVCodecDescriptor* desc = avcodec_descriptor_get(_avCodecContext->codec_id);
    if(!desc)
        throw std::runtime_error("Codec Descriptor is not available.");

    return desc->name;
}

AVCodecID ICodec::getCodecId() const
{
    assert(_avCodecContext != NULL);
    return _avCodecContext->codec_id;
}

int ICodec::getLatency() const
{
    assert(_avCodecContext != NULL);
    return _avCodecContext->delay;
}

std::vector<Option> ICodec::getOptions()
{
    std::vector<Option> optionsArray;
    for(OptionMap::iterator it = _options.begin(); it != _options.end(); ++it)
    {
        optionsArray.push_back(it->second);
    }
    return optionsArray;
}

void ICodec::setCodec(const ECodecType type, const std::string& codecName)
{
    const AVCodecDescriptor* avCodecDescriptor = avcodec_descriptor_get_by_name(codecName.c_str());
    if(!avCodecDescriptor)
    {
        std::string msg("Unable to find codec ");
        msg += codecName;
        throw std::runtime_error(msg);
    }

    setCodec(type, avCodecDescriptor->id);
}

void ICodec::setCodec(const ECodecType type, const AVCodecID codecId)
{
    if(codecId == 0)
    {
        LOG_WARN("Unsupported codec with id 0")
        return;
    }

    if(type == eCodecTypeEncoder)
        _avCodec = avcodec_find_encoder(codecId);
    else if(type == eCodecTypeDecoder)
        _avCodec = avcodec_find_decoder(codecId);

    if(_avCodecContext)
        _avCodecContext->codec = _avCodec;
}

void ICodec::allocateContext()
{
    _avCodecContext = avcodec_alloc_context3(_avCodec);
    if(!_avCodecContext)
    {
        throw std::runtime_error("Unable to allocate the codecContext and set its fields to default values");
    }
    _avCodecContext->codec = _avCodec;
}

void ICodec::loadCodecOptions()
{
    if(_type == eCodecTypeEncoder)
    {
        loadOptions(_options, _avCodecContext, AV_OPT_FLAG_ENCODING_PARAM);
        // load specific options of the codec
        loadOptions(_options, _avCodecContext->priv_data, AV_OPT_FLAG_ENCODING_PARAM);
    }
    else if(_type == eCodecTypeDecoder)
    {
        loadOptions(_options, _avCodecContext, AV_OPT_FLAG_DECODING_PARAM);
        // load specific options of the codec
        loadOptions(_options, _avCodecContext->priv_data, AV_OPT_FLAG_DECODING_PARAM);
    }
}
}
