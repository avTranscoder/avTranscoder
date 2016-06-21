#include "StreamProperties.hpp"

#include <AvTranscoder/properties/util.hpp>
#include <AvTranscoder/properties/JsonWriter.hpp>
#include <AvTranscoder/properties/FileProperties.hpp>
#include <AvTranscoder/data/decoded/Frame.hpp>

#include <stdexcept>

namespace avtranscoder
{

StreamProperties::StreamProperties(const FileProperties& fileProperties, const size_t index)
    : _fileProperties(&fileProperties)
    , _formatContext(&fileProperties.getAVFormatContext())
    , _codecContext(NULL)
    , _codec(NULL)
    , _streamIndex(index)
{
    if(_formatContext)
        detail::fillMetadataDictionnary(_formatContext->streams[index]->metadata, _metadatas);

    if(_formatContext)
    {
        if(_streamIndex > _formatContext->nb_streams)
        {
            std::stringstream ss;
            ss << "Stream at index " << _streamIndex << " does not exist.";
            throw std::runtime_error(ss.str());
        }
        _codecContext = _formatContext->streams[_streamIndex]->codec;
    }

    // find the decoder
    if(_formatContext && _codecContext)
    {
        _codec = avcodec_find_decoder(_codecContext->codec_id);

        if(_codec)
        {
            // load specific options of the codec
            if(avcodec_open2(_codecContext, _codec, NULL) == 0)
            {
                loadOptions(_options, _codecContext);
                avcodec_close(_codecContext);
            }
        }
    }
}

StreamProperties::~StreamProperties()
{
}

size_t StreamProperties::getStreamId() const
{
    if(!_formatContext)
        throw std::runtime_error("unknown format context");
    return _formatContext->streams[_streamIndex]->id;
}

size_t StreamProperties::getBitRate() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");

    // return bit rate of stream if present or VBR mode
    if(_codecContext->bit_rate || _codecContext->rc_max_rate)
        return _codecContext->bit_rate;

    LOG_WARN("The bitrate of the stream '" << _streamIndex << "' of file '" << _formatContext->filename << "' is unknown.")

    const AVMediaType streamType = getStreamType();

    // compute audio bitrate
    if(streamType == AVMEDIA_TYPE_AUDIO)
    {
        LOG_INFO("Compute the audio bitrate (suppose PCM audio data).")

        // warning: this is the way to compute bit rate of PCM audio data
        const int bitsPerSample = av_get_bits_per_sample(_codecContext->codec_id); // 0 if unknown for the given codec
        return _codecContext->sample_rate * _codecContext->channels * bitsPerSample;            
    }

    // compute video bitrate
    if(streamType == AVMEDIA_TYPE_VIDEO)
    {
        LOG_INFO("Compute the video bitrate by decoding the first GOP.")

        if(!_formatContext || !_codec)
            throw std::runtime_error("cannot compute bit rate: unknown format or codec context");
        if(!_codecContext->width || !_codecContext->height)
            throw std::runtime_error("cannot compute bit rate: invalid frame size");

        // discard no frame type when decode
        _codecContext->skip_frame = AVDISCARD_NONE;

        Frame frame;
        AVPacket pkt;
        av_init_packet(&pkt);
        avcodec_open2(_codecContext, _codec, NULL);

        int gotFrame = 0;
        size_t nbDecodedFrames = 0;
        int gopFramesSize = 0;
        int positionOfFirstKeyFrame = -1;
        int positionOfLastKeyFrame = -1;

        while(!av_read_frame(const_cast<AVFormatContext*>(_formatContext), &pkt))
        {
            if(pkt.stream_index == (int)_streamIndex)
            {
                avcodec_decode_video2(_codecContext, &frame.getAVFrame(), &gotFrame, &pkt);
                if(gotFrame)
                {
                    // check distance between key frames
                    AVFrame& avFrame = frame.getAVFrame();
                    if(avFrame.pict_type == AV_PICTURE_TYPE_I)
                    {
                        if(positionOfFirstKeyFrame == -1)
                            positionOfFirstKeyFrame = nbDecodedFrames;
                        else
                            positionOfLastKeyFrame = nbDecodedFrames;
                    }
                    ++nbDecodedFrames;

                    // added size of all frames of the same gop
                    if(positionOfLastKeyFrame == -1)
                    {
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(54, 7, 100)
                        gopFramesSize += frame.getEncodedSize();
#else
                        gopFramesSize += pkt.size;
#endif
                    }
                }
            }
            av_free_packet(&pkt);
            if(positionOfFirstKeyFrame != -1 && positionOfLastKeyFrame != -1)
                break;
        }
        // Close a given AVCodecContext and free all the data associated with it (but not the AVCodecContext itself)
        avcodec_close(_codecContext);

        // Returns at the beginning of the stream
        const_cast<FormatContext*>(&_fileProperties->getFormatContext())->seek(0, AVSEEK_FLAG_BYTE);

        const size_t gopSize = positionOfLastKeyFrame - positionOfFirstKeyFrame;
        if(gopSize > 0)
        {
            const float fps = av_q2d(_formatContext->streams[_streamIndex]->avg_frame_rate);
            return (gopFramesSize / gopSize) * 8 * fps;
        }
    }

    return 0;
}

Rational StreamProperties::getTimeBase() const
{
    if(!_formatContext)
        throw std::runtime_error("unknown format context");
    return _formatContext->streams[_streamIndex]->time_base;
}

float StreamProperties::getDuration() const
{
    const Rational timeBase = getTimeBase();
    const size_t duration = _formatContext->streams[_streamIndex]->duration;
    if(duration == (size_t)AV_NOPTS_VALUE)
    {
        LOG_WARN("The duration of the stream '" << _streamIndex << "' of file '" << _formatContext->filename << "' is unknown.")
        if(_fileProperties->isRawFormat())
        {
            LOG_INFO("Get the stream bitrate to compute the duration.")
            const size_t bitRate = getBitRate();
            if(bitRate)
            {
                LOG_INFO("Get the file size to compute the duration.")
                return _fileProperties->getFileSize() / bitRate * 8;
            }
        }
        return 0;
    }
    return av_q2d(timeBase) * duration;
}

AVMediaType StreamProperties::getStreamType() const
{
    if(!_formatContext)
        throw std::runtime_error("unknown format context");
    return _formatContext->streams[_streamIndex]->codec->codec_type;
}

size_t StreamProperties::getCodecId() const
{
    if(!_codecContext)
        throw std::runtime_error("unknown codec context");
    return _codecContext->codec_id;
}

std::string StreamProperties::getCodecName() const
{
    if(!_codecContext || !_codec)
        throw std::runtime_error("unknown codec");

    if(_codec->capabilities & CODEC_CAP_TRUNCATED)
        _codecContext->flags |= CODEC_FLAG_TRUNCATED;

    if(!_codec->name)
        throw std::runtime_error("unknown codec name");

    return std::string(_codec->name);
}

std::string StreamProperties::getCodecLongName() const
{
    if(!_codecContext || !_codec)
        throw std::runtime_error("unknown codec");

    if(_codec->capabilities & CODEC_CAP_TRUNCATED)
        _codecContext->flags |= CODEC_FLAG_TRUNCATED;

    if(!_codec->long_name)
        throw std::runtime_error("unknown codec long name");

    return std::string(_codec->long_name);
}

std::vector<Option> StreamProperties::getCodecOptions()
{
    std::vector<Option> optionsArray;
    for(OptionMap::iterator it = _options.begin(); it != _options.end(); ++it)
    {
        optionsArray.push_back(it->second);
    }
    return optionsArray;
}

PropertyVector StreamProperties::asVector() const
{
    PropertyVector propertyVector;
    return fillVector(propertyVector);
}

PropertyVector& StreamProperties::fillVector(PropertyVector& data) const
{
    addProperty(data, "streamId", &StreamProperties::getStreamId);
    addProperty(data, "streamIndex", &StreamProperties::getStreamIndex);
    addProperty(data, "bitRate", &StreamProperties::getBitRate);
    addProperty(data, "timeBase", &StreamProperties::getTimeBase);
    addProperty(data, "duration", &StreamProperties::getDuration);
    addProperty(data, "codecId", &StreamProperties::getCodecId);
    addProperty(data, "codecName", &StreamProperties::getCodecName);
    addProperty(data, "codecLongName", &StreamProperties::getCodecLongName);

    for(size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex)
    {
        detail::add(data, _metadatas.at(metadataIndex).first, _metadatas.at(metadataIndex).second);
    }

    return data;
}

PropertyMap StreamProperties::asMap() const
{
    PropertyMap dataMap;

    PropertyVector dataVector(asVector());
    for(PropertyVector::const_iterator it = dataVector.begin(); it != dataVector.end(); ++it)
    {
        dataMap.insert(std::make_pair(it->first, it->second));
    }

    return dataMap;
}

std::string StreamProperties::asJson() const
{
    json::JsonObjectStreamWriter writer;
    PropertyMap properties = asMap();
    for(PropertyMap::iterator it = properties.begin(); it != properties.end(); ++it)
        writer << std::make_pair(it->first.c_str(), it->second.c_str());
    return writer.build();
}

std::ostream& operator<<(std::ostream& flux, const StreamProperties& streamProperties)
{
    flux << std::left;
    flux << detail::separator << " Stream " << detail::separator << std::endl;

    PropertyVector properties = streamProperties.asVector();
    for(PropertyVector::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        flux << std::setw(detail::keyWidth) << it->first << ": " << it->second << std::endl;
    }

    return flux;
}
}
