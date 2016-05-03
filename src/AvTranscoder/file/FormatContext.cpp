#include "FormatContext.hpp"

#include <stdexcept>
#include <sstream>

namespace avtranscoder
{

FormatContext::FormatContext(const std::string& filename, int req_flags, AVDictionary** options)
    : _avFormatContext(NULL)
    , _flags(req_flags)
    , _options()
    , _isOpen(false)
{
    int ret = avformat_open_input(&_avFormatContext, filename.c_str(), NULL, options);
    if(ret < 0)
    {
        std::string msg = "Unable to open file ";
        msg += filename;
        msg += ": ";
        msg += getDescriptionFromErrorCode(ret);
        throw std::ios_base::failure(msg);
    }
    _isOpen = true;

    loadOptions(_options, _avFormatContext, req_flags);
    // when demuxing, priv_data of AVFormatContext is set by avformat_open_input()
    if(_avFormatContext->iformat->priv_class)
        loadOptions(_options, _avFormatContext->priv_data, req_flags);
}

FormatContext::FormatContext(int req_flags)
    : _avFormatContext(NULL)
    , _flags(req_flags)
    , _options()
    , _isOpen(false)
{
    _avFormatContext = avformat_alloc_context();
    loadOptions(_options, _avFormatContext, req_flags);
}

FormatContext::~FormatContext()
{
    if(!_avFormatContext)
        return;

    if(_isOpen)
        avformat_close_input(&_avFormatContext);
    else
        avformat_free_context(_avFormatContext);
    _avFormatContext = NULL;
}

void FormatContext::findStreamInfo(AVDictionary** options)
{
    const int err = avformat_find_stream_info(_avFormatContext, options);
    if(err < 0)
    {
        throw std::ios_base::failure("Unable to find stream informations: " + getDescriptionFromErrorCode(err));
    }
}

void FormatContext::openRessource(const std::string& url, int flags)
{
    if((_avFormatContext->flags & AVFMT_NOFILE) == AVFMT_NOFILE)
        return;

    const int err = avio_open2(&_avFormatContext->pb, url.c_str(), flags, NULL, NULL);
    if(err < 0)
    {
        throw std::ios_base::failure("Error when opening output format: " + getDescriptionFromErrorCode(err));
    }
}

void FormatContext::closeRessource()
{
    if((_avFormatContext->flags & AVFMT_NOFILE) == AVFMT_NOFILE)
        return;

    const int err = avio_close(_avFormatContext->pb);
    if(err < 0)
    {
        throw std::ios_base::failure("Error when close output format: " + getDescriptionFromErrorCode(err));
    }
}

void FormatContext::writeHeader(AVDictionary** options)
{
    const int ret = avformat_write_header(_avFormatContext, options);
    if(ret != 0)
    {
        throw std::runtime_error("Could not write header: " + getDescriptionFromErrorCode(ret));
    }
    // when muxing, priv_data of AVFormatContext is set by avformat_write_header()
    if(_avFormatContext->oformat->priv_class)
        loadOptions(_options, _avFormatContext->priv_data, _flags);
}

void FormatContext::writeFrame(AVPacket& packet, bool interleaved)
{
    int ret = 0;
    if(interleaved)
        ret = av_interleaved_write_frame(_avFormatContext, &packet);
    else
    {
        // returns 1 if flushed and there is no more data to flush
        ret = av_write_frame(_avFormatContext, &packet);
    }

    if(ret < 0)
    {
        throw std::runtime_error("Error when writting packet in stream: " + getDescriptionFromErrorCode(ret));
    }
}

void FormatContext::writeTrailer()
{
    const int ret = av_write_trailer(_avFormatContext);
    if(ret != 0)
    {
        throw std::runtime_error("Could not write trailer: " + getDescriptionFromErrorCode(ret));
    }
}

void FormatContext::addMetaData(const std::string& key, const std::string& value)
{
    const int ret = av_dict_set(&_avFormatContext->metadata, key.c_str(), value.c_str(), 0);
    if(ret < 0)
    {
        LOG_ERROR(getDescriptionFromErrorCode(ret))
    }
}

AVStream& FormatContext::addAVStream(const AVCodec& avCodec)
{
    // Need const_cast<AVCodec*> for libav versions < 54.34.
    AVStream* stream = avformat_new_stream(_avFormatContext, const_cast<AVCodec*>(&avCodec));
    if(stream == NULL)
    {
        throw std::runtime_error("Unable to add new video stream");
    }
    return *stream;
}

bool FormatContext::seek(const uint64_t position, const int flag)
{
    LOG_INFO("Seek in '" << _avFormatContext->filename << "' at " << position << " with flag '"<< flag << "'")
    const int err = av_seek_frame(_avFormatContext, -1, position, flag);
    if(err < 0)
    {
        LOG_ERROR("Error when seek at " << position << " (in AV_TIME_BASE units) in file")
        LOG_ERROR(getDescriptionFromErrorCode(err))
        return false;
    }
    return true;
}

std::vector<Option> FormatContext::getOptions()
{
    std::vector<Option> optionsArray;
    for(OptionMap::iterator it = _options.begin(); it != _options.end(); ++it)
    {
        optionsArray.push_back(it->second);
    }
    return optionsArray;
}

AVStream& FormatContext::getAVStream(size_t index) const
{
    if(index >= getNbStreams())
    {
        std::stringstream msg;
        msg << "Can't acces stream at index ";
        msg << index;
        throw std::runtime_error(msg.str());
    }
    return *_avFormatContext->streams[index];
}

void FormatContext::setFilename(const std::string& filename)
{
    strcpy(&_avFormatContext->filename[0], filename.c_str());
}

void FormatContext::setOutputFormat(const std::string& filename, const std::string& shortName, const std::string& mimeType)
{
    AVOutputFormat* oformat = av_guess_format(shortName.c_str(), filename.c_str(), mimeType.c_str());
    if(!oformat)
    {
        std::string msg("Unable to find format for ");
        msg += filename;
        if(!shortName.empty())
        {
            msg += ", formatName = ";
            msg += shortName;
        }
        if(!mimeType.empty())
        {
            msg += ", mimeType = ";
            msg += mimeType;
        }
        throw std::ios_base::failure(msg);
    }

    _avFormatContext->oformat = oformat;
}
}
