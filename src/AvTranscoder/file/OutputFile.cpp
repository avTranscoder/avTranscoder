#include "OutputFile.hpp"

#include <AvTranscoder/util.hpp>

#include <stdexcept>
#include <libavutil/channel_layout.h>

#ifndef FF_INPUT_BUFFER_PADDING_SIZE
#define FF_INPUT_BUFFER_PADDING_SIZE 16
#endif

namespace avtranscoder
{

OutputFile::OutputFile(const std::string& filename, const std::string& formatName, const std::string& mimeType)
    : _formatContext(AV_OPT_FLAG_ENCODING_PARAM)
    , _outputStreams()
    , _frameCount()
    , _previousProcessedStreamDuration(0.0)
    , _profileOptions(NULL)
{
    _formatContext.setFilename(filename);
    _formatContext.setOutputFormat(filename, formatName, mimeType);
}

OutputFile::~OutputFile()
{
    for(std::vector<OutputStream*>::iterator it = _outputStreams.begin(); it != _outputStreams.end(); ++it)
    {
        delete(*it);
    }
    av_dict_free(&_profileOptions);
}

IOutputStream& OutputFile::addVideoStream(const VideoCodec& videoDesc)
{
    AVStream& stream = _formatContext.addAVStream(videoDesc.getAVCodec());

    stream.codecpar->codec_type = videoDesc.getAVCodecContext().codec_type;
    stream.codecpar->codec_id = videoDesc.getAVCodecContext().codec_id;
    stream.codecpar->codec_tag = videoDesc.getAVCodecContext().codec_tag;

    stream.codecpar->width = videoDesc.getAVCodecContext().width;
    stream.codecpar->height = videoDesc.getAVCodecContext().height;
    stream.codecpar->bit_rate = videoDesc.getAVCodecContext().bit_rate;
    stream.codecpar->format = videoDesc.getAVCodecContext().pix_fmt;
    stream.codecpar->profile = videoDesc.getAVCodecContext().profile;
    stream.codecpar->level = videoDesc.getAVCodecContext().level;
    stream.codecpar->field_order = videoDesc.getAVCodecContext().field_order;

    stream.codecpar->color_space = videoDesc.getAVCodecContext().colorspace;
    stream.codecpar->color_primaries = videoDesc.getAVCodecContext().color_primaries;
    stream.codecpar->color_range = videoDesc.getAVCodecContext().color_range;
    stream.codecpar->color_trc = videoDesc.getAVCodecContext().color_trc;
    stream.codecpar->chroma_location = videoDesc.getAVCodecContext().chroma_sample_location;

    setOutputStream(stream, videoDesc);

    // need to set the time_base on the AVCodecContext and the AVStream
    // compensating the frame rate with the ticks_per_frame and keeping
    // a coherent reading speed.
    av_reduce(&stream.time_base.num, &stream.time_base.den,
              videoDesc.getAVCodecContext().time_base.num * videoDesc.getAVCodecContext().ticks_per_frame,
              videoDesc.getAVCodecContext().time_base.den, INT_MAX);

    OutputStream* outputStream = new OutputStream(*this, _formatContext.getNbStreams() - 1);
    _outputStreams.push_back(outputStream);

    return *outputStream;
}

IOutputStream& OutputFile::addAudioStream(const AudioCodec& audioDesc)
{
    AVStream& stream = _formatContext.addAVStream(audioDesc.getAVCodec());

    stream.codecpar->codec_type = audioDesc.getAVCodecContext().codec_type;
    stream.codecpar->codec_id = audioDesc.getAVCodecContext().codec_id;
    stream.codecpar->codec_tag = audioDesc.getAVCodecContext().codec_tag;

    stream.codecpar->sample_rate = audioDesc.getAVCodecContext().sample_rate;
    stream.codecpar->channels = audioDesc.getAVCodecContext().channels;
    stream.codecpar->channel_layout = audioDesc.getAVCodecContext().channel_layout;
    stream.codecpar->format = audioDesc.getAVCodecContext().sample_fmt;
    stream.codecpar->frame_size = audioDesc.getAVCodecContext().frame_size;

    setOutputStream(stream, audioDesc);

    // need to set the time_base on the AVCodecContext of the AVStream
    av_reduce(&stream.time_base.num, &stream.time_base.den, audioDesc.getAVCodecContext().time_base.num,
              audioDesc.getAVCodecContext().time_base.den, INT_MAX);

    OutputStream* outputStream = new OutputStream(*this, _formatContext.getNbStreams() - 1);
    _outputStreams.push_back(outputStream);

    return *outputStream;
}

IOutputStream& OutputFile::addCustomStream(const ICodec& iCodecDesc)
{
    AVStream& stream = _formatContext.addAVStream(iCodecDesc.getAVCodec());

    stream.codecpar->codec_type = iCodecDesc.getAVCodecContext().codec_type;
    stream.codecpar->codec_id = iCodecDesc.getAVCodecContext().codec_id;
    stream.codecpar->codec_tag = iCodecDesc.getAVCodecContext().codec_tag;

    stream.codecpar->sample_rate = 48000;
    stream.codecpar->channels = 1;
    stream.codecpar->channel_layout = AV_CH_LAYOUT_MONO;
    stream.codecpar->format = AV_SAMPLE_FMT_S32;
    stream.codecpar->frame_size = 1920;

    // need to set the time_base on the AVCodecContext of the AVStream
    av_reduce(&stream.time_base.num, &stream.time_base.den, 1, 1, INT_MAX);

    OutputStream* outputStream = new OutputStream(*this, _formatContext.getNbStreams() - 1);
    _outputStreams.push_back(outputStream);

    return *outputStream;
}

IOutputStream& OutputFile::addDataStream(const DataCodec& dataDesc)
{
    _formatContext.addAVStream(dataDesc.getAVCodec());

    OutputStream* outputStream = new OutputStream(*this, _formatContext.getNbStreams() - 1);
    _outputStreams.push_back(outputStream);

    return *outputStream;
}

IOutputStream& OutputFile::getStream(const size_t streamIndex)
{
    if(streamIndex >= _outputStreams.size())
    {
        std::stringstream msg;
        msg << "Unable to get the stream ";
        msg << streamIndex;
        msg << ": the OutputFile '";
        msg << getFilename();
        msg << "' has only ";
        msg << _outputStreams.size();
        msg << " streams.";
        throw std::runtime_error(msg.str());
    }
    return *_outputStreams.at(streamIndex);
}

std::string OutputFile::getFilename() const
{
    return std::string(_formatContext.getAVFormatContext().url);
}

std::string OutputFile::getFormatName() const
{
    if(_formatContext.getAVOutputFormat().name == NULL)
    {
        LOG_WARN("Unknown muxer format name of '" << getFilename() << "'.")
        return "";
    }
    return std::string(_formatContext.getAVOutputFormat().name);
}

std::string OutputFile::getFormatLongName() const
{
    if(_formatContext.getAVOutputFormat().long_name == NULL)
    {
        LOG_WARN("Unknown muxer format long name of '" << getFilename() << "'.")
        return "";
    }
    return std::string(_formatContext.getAVOutputFormat().long_name);
}

std::string OutputFile::getFormatMimeType() const
{
    if(_formatContext.getAVOutputFormat().mime_type == NULL)
    {
        LOG_WARN("Unknown muxer format mime type of '" << getFilename() << "'.")
        return "";
    }
    return std::string(_formatContext.getAVOutputFormat().mime_type);
}

bool OutputFile::beginWrap()
{
    LOG_DEBUG("Begin wrap of OutputFile")

    _formatContext.openRessource(getFilename(), AVIO_FLAG_WRITE);
    _formatContext.writeHeader(&_profileOptions);

    // set specific wrapping options
    setupRemainingWrappingOptions();

    _frameCount.clear();
    _frameCount.resize(_outputStreams.size(), 0);

    return true;
}

IOutputStream::EWrappingStatus OutputFile::wrap(const CodedData& data, const size_t streamIndex)
{
    if(!data.getSize())
        return IOutputStream::eWrappingSkip;

    LOG_DEBUG("Wrap on stream " << streamIndex << " (" << data.getSize() << " bytes for frame "
                                << _frameCount.at(streamIndex) << ")")

    // Packet to wrap
    AVPacket packet = *av_packet_alloc();
    packet.stream_index = streamIndex;
    packet.data = (uint8_t*)data.getData();
    packet.size = data.getSize();
    packet.flags = data.getAVPacket().flags;

    // copy timing information
    if(!_outputStreams.at(streamIndex)->isPTSGenerated())
    {
        if(data.getAVStream() != NULL)
        {
            const AVRational& srcTimeBase = data.getAVStream()->time_base;
            const AVRational& dstTimeBase = _formatContext.getAVStream(streamIndex).time_base;
            // duration
            packet.duration = av_rescale_q(data.getAVPacket().duration, srcTimeBase, dstTimeBase);
            // pts
            if(data.getAVPacket().pts != AV_NOPTS_VALUE)
                packet.pts = av_rescale_q(data.getAVPacket().pts, srcTimeBase, dstTimeBase);
            else
                packet.pts = AV_NOPTS_VALUE;
            // dts
            packet.dts = av_rescale_q(data.getAVPacket().dts, srcTimeBase, dstTimeBase);
        }
        // add stream PTS if already incremented
        // const int currentStreamPTS = _outputStreams.at(streamIndex)->getStreamPTS();
        // if(packet.pts != AV_NOPTS_VALUE && packet.pts < currentStreamPTS)
        // {
        //     packet.pts += currentStreamPTS;
        //     packet.dts += currentStreamPTS;
        // }
    }

    // copy duration of packet wrapped
    // @see OutputStream
    const_cast<CodedData&>(data).getAVPacket().duration = packet.duration;

    // Write packet
    _formatContext.writeFrame(packet);

    const double currentStreamDuration = _outputStreams.at(streamIndex)->getStreamDuration();
    if(currentStreamDuration < _previousProcessedStreamDuration)
    {
        LOG_DEBUG("The output stream " << streamIndex << " is strictly shorter than the previous duration saved ("
                                       << currentStreamDuration << "s < " << _previousProcessedStreamDuration
                                       << "s): wait for more data.")
        return IOutputStream::eWrappingWaitingForData;
    }

    _previousProcessedStreamDuration = currentStreamDuration;
    _frameCount.at(streamIndex)++;

    return IOutputStream::eWrappingSuccess;
}

bool OutputFile::endWrap()
{
    LOG_DEBUG("End wrap of OutputFile")

    _formatContext.writeTrailer();
    _formatContext.closeRessource();
    return true;
}

void OutputFile::addMetadata(const PropertyVector& data)
{
    for(PropertyVector::const_iterator it = data.begin(); it != data.end(); ++it)
    {
        addMetadata(it->first, it->second);
    }
}

void OutputFile::addMetadata(const std::string& key, const std::string& value)
{
    _formatContext.addMetaData(key, value);
}

void OutputFile::setupWrapping(const ProfileLoader::Profile& profile)
{
    // check the given profile
    const bool isValid = ProfileLoader::checkFormatProfile(profile);
    if(!isValid)
    {
        const std::string msg("Invalid format profile to setup wrapping.");
        LOG_ERROR(msg)
        throw std::runtime_error(msg);
    }

    if(!profile.empty())
    {
        LOG_INFO("Setup wrapping with:\n" << profile)
    }

    // check if output format indicated is valid with the filename extension
    if(!av_guess_format(profile.find(constants::avProfileFormat)->second.c_str(), getFilename().c_str(), NULL))
    {
        throw std::runtime_error("Invalid format according to the file extension.");
    }
    // set output format
    _formatContext.setOutputFormat(getFilename(), profile.find(constants::avProfileFormat)->second);

    // set common wrapping options
    setupWrappingOptions(profile);
}

void OutputFile::setupWrappingOptions(const ProfileLoader::Profile& profile)
{
    // set format options
    for(ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it)
    {
        if((*it).first == constants::avProfileIdentificator || (*it).first == constants::avProfileIdentificatorHuman ||
           (*it).first == constants::avProfileType || (*it).first == constants::avProfileFormat)
            continue;

        try
        {
            Option& formatOption = _formatContext.getOption((*it).first);
            formatOption.setString((*it).second);
        }
        catch(std::exception& e)
        {
            LOG_INFO("OutputFile - option " << (*it).first << " will be saved to be called when beginWrap")
            av_dict_set(&_profileOptions, (*it).first.c_str(), (*it).second.c_str(), 0);
        }
    }
}

void OutputFile::setupRemainingWrappingOptions()
{
    // set specific format options
    AVDictionaryEntry* optionEntry = NULL;
    while((optionEntry = av_dict_get(_profileOptions, "", optionEntry, AV_DICT_IGNORE_SUFFIX)))
    {
        const std::string optionKey(optionEntry->key);
        const std::string optionValue(optionEntry->value);

        if(optionKey == constants::avProfileIdentificator || optionKey == constants::avProfileIdentificatorHuman ||
           optionKey == constants::avProfileType || optionKey == constants::avProfileFormat)
            continue;

        try
        {
            Option& formatOption = _formatContext.getOption(optionKey);
            formatOption.setString(optionValue);
        }
        catch(std::exception& e)
        {
            LOG_WARN("OutputFile - can't set option " << optionKey << " to " << optionValue << ": " << e.what())
        }
    }
}

void OutputFile::setOutputStream(AVStream& avStream, const ICodec& codec)
{
#if LIBAVCODEC_VERSION_MAJOR < 59
    // depending on the format, place global headers in extradata instead of every keyframe
    if(_formatContext.getAVOutputFormat().flags & AVFMT_GLOBALHEADER)
    {
#ifdef AV_CODEC_FLAG_GLOBAL_HEADER
        avStream.codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
#else
        avStream.codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
#endif
    }

    // if the codec is experimental, allow it
#ifdef AV_CODEC_CAP_EXPERIMENTAL
    if(codec.getAVCodec().capabilities & AV_CODEC_CAP_EXPERIMENTAL)
#else
    if(codec.getAVCodec().capabilities & CODEC_CAP_EXPERIMENTAL)
#endif
    {
        LOG_WARN("This codec is considered experimental by libav/ffmpeg:" << codec.getCodecName());
        avStream.codec->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    }
#endif

    // some codecs need/can use extradata to decode
    uint8_t* srcExtradata = codec.getAVCodecContext().extradata;
    const int srcExtradataSize = codec.getAVCodecContext().extradata_size;
    avStream.codecpar->extradata = (uint8_t*)av_malloc(srcExtradataSize + FF_INPUT_BUFFER_PADDING_SIZE);
    memcpy(avStream.codecpar->extradata, srcExtradata, srcExtradataSize);
    memset(((uint8_t*)avStream.codecpar->extradata) + srcExtradataSize, 0, FF_INPUT_BUFFER_PADDING_SIZE);
    avStream.codecpar->extradata_size = codec.getAVCodecContext().extradata_size;
}
}
