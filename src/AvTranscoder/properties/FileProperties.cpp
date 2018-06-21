#include "FileProperties.hpp"

#include <AvTranscoder/properties/util.hpp>
#include <AvTranscoder/properties/JsonWriter.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>

#include <stdexcept>
#include <sstream>
#include <fstream>

namespace avtranscoder
{

FileProperties::FileProperties(const InputFile& file)
    : _file(file)
    , _formatContext(&file.getFormatContext())
    , _avFormatContext(&file.getFormatContext().getAVFormatContext())
    , _streamsProperties()
    , _videoStreams()
    , _audioStreams()
    , _dataStreams()
    , _subtitleStreams()
    , _attachementStreams()
    , _unknownStreams()
{
    if(_avFormatContext)
        detail::fillMetadataDictionnary(_avFormatContext->metadata, _metadatas);

    NoDisplayProgress progress;
    extractStreamProperties(progress, eAnalyseLevelHeader);
}

void FileProperties::extractStreamProperties(IProgress& progress, const EAnalyseLevel level)
{
    // Returns at the beginning of the stream before any deep analysis
    if(level > eAnalyseLevelHeader && ! isRawFormat())
        const_cast<InputFile&>(_file).seekAtFrame(0, AVSEEK_FLAG_BACKWARD);

    // clear properties
    _videoStreams.clear();
    _audioStreams.clear();
    _dataStreams.clear();
    _subtitleStreams.clear();
    _attachementStreams.clear();
    _unknownStreams.clear();

    // reload properties
    for(size_t streamIndex = 0; streamIndex < _formatContext->getNbStreams(); ++streamIndex)
    {
        switch(_formatContext->getAVStream(streamIndex).codec->codec_type)
        {
            case AVMEDIA_TYPE_VIDEO:
            {
                VideoProperties properties(*this, streamIndex, progress, level);
                _videoStreams.push_back(properties);
                break;
            }
            case AVMEDIA_TYPE_AUDIO:
            {
                AudioProperties properties(*this, streamIndex);
                _audioStreams.push_back(properties);
                break;
            }
            case AVMEDIA_TYPE_DATA:
            {
                DataProperties properties(*this, streamIndex);
                _dataStreams.push_back(properties);
                break;
            }
            case AVMEDIA_TYPE_SUBTITLE:
            {
                SubtitleProperties properties(*this, streamIndex);
                _subtitleStreams.push_back(properties);
                break;
            }
            case AVMEDIA_TYPE_ATTACHMENT:
            {
                AttachementProperties properties(*this, streamIndex);
                _attachementStreams.push_back(properties);
                break;
            }
            case AVMEDIA_TYPE_UNKNOWN:
            {
                UnknownProperties properties(*this, streamIndex);
                _unknownStreams.push_back(properties);
                break;
            }
            case AVMEDIA_TYPE_NB:
            {
                break;
            }
        }
    }

    // clear streams
    _streamsProperties.clear();

    // once the streams vectors are filled, add their references the base streams vector
    for(size_t streamIndex = 0; streamIndex < _videoStreams.size(); ++streamIndex)
    {
        const size_t videoStreamIndex = _videoStreams.at(streamIndex).getStreamIndex();
        _streamsProperties[videoStreamIndex] = &_videoStreams.at(streamIndex);
    }

    for(size_t streamIndex = 0; streamIndex < _audioStreams.size(); ++streamIndex)
    {
        const size_t audioStreamIndex = _audioStreams.at(streamIndex).getStreamIndex();
        _streamsProperties[audioStreamIndex] = &_audioStreams.at(streamIndex);
    }

    for(size_t streamIndex = 0; streamIndex < _dataStreams.size(); ++streamIndex)
    {
        const size_t dataStreamIndex = _dataStreams.at(streamIndex).getStreamIndex();
        _streamsProperties[dataStreamIndex] = &_dataStreams.at(streamIndex);
    }

    for(size_t streamIndex = 0; streamIndex < _subtitleStreams.size(); ++streamIndex)
    {
        const size_t subtitleStreamIndex = _subtitleStreams.at(streamIndex).getStreamIndex();
        _streamsProperties[subtitleStreamIndex] = &_subtitleStreams.at(streamIndex);
    }

    for(size_t streamIndex = 0; streamIndex < _attachementStreams.size(); ++streamIndex)
    {
        const size_t attachementStreamIndex = _attachementStreams.at(streamIndex).getStreamIndex();
        _streamsProperties[attachementStreamIndex] = &_attachementStreams.at(streamIndex);
    }

    for(size_t streamIndex = 0; streamIndex < _unknownStreams.size(); ++streamIndex)
    {
        const size_t unknownStreamIndex = _unknownStreams.at(streamIndex).getStreamIndex();
        _streamsProperties[unknownStreamIndex] = &_unknownStreams.at(streamIndex);
    }

    // Returns at the beginning of the stream after any deep analysis
    if(level > eAnalyseLevelHeader && ! isRawFormat())
        const_cast<InputFile&>(_file).seekAtFrame(0, AVSEEK_FLAG_BACKWARD);
}

std::string FileProperties::getFilename() const
{
    if(!_avFormatContext || !_avFormatContext->filename)
        throw std::runtime_error("unknown file name");
    return _avFormatContext->filename;
}

std::string FileProperties::getFormatName() const
{
    if(!_avFormatContext || !_avFormatContext->iformat || !_avFormatContext->iformat->name)
        throw std::runtime_error("unknown format name");
    return _avFormatContext->iformat->name;
}

std::string FileProperties::getFormatLongName() const
{
    if(!_avFormatContext || !_avFormatContext->iformat || !_avFormatContext->iformat->long_name)
        throw std::runtime_error("unknown format long name");
    return _avFormatContext->iformat->long_name;
}

bool FileProperties::isRawFormat() const
{
    if(getNbStreams() != 1)
        return false;
    // the format name should be the same as the codec name
    if(getFormatName() == getStreamProperties().at(0)->getCodecName())
        return true;
    return false;
}

std::string FileProperties::getFormatMimeType() const
{
#if LIBAVFORMAT_VERSION_MAJOR <= 55
    throw std::runtime_error("cannot get mime type format: libavformat library has a major version <= 55.");
#else
    if(_avFormatContext->iformat->mime_type == NULL)
        throw std::runtime_error("Unknown demuxer format mime type");
    return std::string(_avFormatContext->iformat->mime_type);
#endif
}

size_t FileProperties::getProgramsCount() const
{
    if(!_avFormatContext)
        throw std::runtime_error("unknown format context");
    return _avFormatContext->nb_programs;
}

double FileProperties::getStartTime() const
{
    if(!_avFormatContext)
        throw std::runtime_error("unknown format context");
    return 1.0 * (unsigned int)_avFormatContext->start_time / AV_TIME_BASE;
}

float FileProperties::getDuration() const
{
    if(!_avFormatContext)
        throw std::runtime_error("unknown format context");
    const size_t duration = _avFormatContext->duration;
    if(duration == (size_t)AV_NOPTS_VALUE)
        return 0;
    return 1.0 * _avFormatContext->duration / AV_TIME_BASE;
}

size_t FileProperties::getBitRate() const
{
    if(!_avFormatContext)
        throw std::runtime_error("unknown format context");
    return _avFormatContext->bit_rate;
}

unsigned __int64 FileProperties::getFileSize() const
{
    std::ifstream in(getFilename().c_str(), std::ios::binary | std::ios::ate);
    return in.tellg();
}

size_t FileProperties::getPacketSize() const
{
    if(!_avFormatContext)
        throw std::runtime_error("unknown format context");
    return _avFormatContext->packet_size;
}

const avtranscoder::StreamProperties& FileProperties::getStreamPropertiesWithIndex(const size_t streamIndex) const
{
    avtranscoder::StreamProperties* properties = _streamsProperties.find(streamIndex)->second;
    if(properties)
        return *properties;
    std::stringstream msg;
    msg << "No stream properties correspond to stream at index ";
    msg << streamIndex;
    throw std::runtime_error(msg.str());
}

const std::vector<avtranscoder::StreamProperties*> FileProperties::getStreamProperties() const
{
    std::vector<avtranscoder::StreamProperties*> streams;
    for(std::map<size_t, StreamProperties*>::const_iterator it = _streamsProperties.begin(); it != _streamsProperties.end(); ++it)
    {
        streams.push_back(it->second);
    }
    return streams;
}

size_t FileProperties::getNbStreams() const
{
    if(!_avFormatContext)
        throw std::runtime_error("unknown format context");
    return _avFormatContext->nb_streams;
}

PropertyVector FileProperties::asVector() const
{
    PropertyVector propertyVector;
    return fillVector(propertyVector);
}

PropertyVector& FileProperties::fillVector(PropertyVector& data) const
{
    addProperty(data, "filename", &FileProperties::getFilename);
    addProperty(data, "formatName", &FileProperties::getFormatName);
    addProperty(data, "formatLongName", &FileProperties::getFormatLongName);
    addProperty(data, "mimeType", &FileProperties::getFormatMimeType);
    addProperty(data, "rawFormat", &FileProperties::isRawFormat);

    addProperty(data, "startTime", &FileProperties::getStartTime);
    addProperty(data, "duration", &FileProperties::getDuration);
    addProperty(data, "bitrate", &FileProperties::getBitRate);
    addProperty(data, "fileSize", &FileProperties::getFileSize);
    addProperty(data, "packetSize", &FileProperties::getPacketSize);
    addProperty(data, "numberOfStreams", &FileProperties::getNbStreams);
    addProperty(data, "numberOfPrograms", &FileProperties::getProgramsCount);

    detail::add(data, "numberOfVideoStreams", getNbVideoStreams());
    detail::add(data, "numberOfAudioStreams", getNbAudioStreams());
    detail::add(data, "numberOfDataStreams", getNbDataStreams());
    detail::add(data, "numberOfSubtitleStreams", getNbSubtitleStreams());
    detail::add(data, "numberOfAttachementStreams", getNbAttachementStreams());
    detail::add(data, "numberOfUnknownStreams", getNbUnknownStreams());

    for(size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex)
    {
        detail::add(data, _metadatas.at(metadataIndex).first, _metadatas.at(metadataIndex).second);
    }

    return data;
}

PropertyMap FileProperties::asMap() const
{
    PropertyMap dataMap;

    PropertyVector dataVector(asVector());
    for(PropertyVector::const_iterator it = dataVector.begin(); it != dataVector.end(); ++it)
    {
        dataMap.insert(std::make_pair(it->first, it->second));
    }

    return dataMap;
}

std::string FileProperties::asJson() const
{
    json::JsonObjectStreamWriter writer;
    PropertyMap properties = asMap();
    for(PropertyMap::iterator it = properties.begin(); it != properties.end(); ++it)
        writer << std::make_pair(it->first.c_str(), it->second.c_str());
    return writer.build();
}

std::string FileProperties::allPropertiesAsJson() const
{
    json::JsonObjectStreamWriter writer;
    {
        // format
        json::JsonArrayStreamWriter format;
        format << asJson();
        writer << std::make_pair("format", format.build());
    }
    {
        // video streams
        json::JsonArrayStreamWriter video;
        for(std::vector<avtranscoder::VideoProperties>::const_iterator it = _videoStreams.begin(); it != _videoStreams.end();
            ++it)
        {
            video << it->asJson();
        }
        writer << std::make_pair("video", video.build());
    }
    {
        // audio streams
        json::JsonArrayStreamWriter audio;
        for(std::vector<avtranscoder::AudioProperties>::const_iterator it = _audioStreams.begin(); it != _audioStreams.end();
            ++it)
        {
            audio << it->asJson();
        }
        writer << std::make_pair("audio", audio.build());
    }
    {
        // data streams
        json::JsonArrayStreamWriter data;
        for(std::vector<avtranscoder::DataProperties>::const_iterator it = _dataStreams.begin(); it != _dataStreams.end();
            ++it)
        {
            data << it->asJson();
        }
        writer << std::make_pair("data", data.build());
    }
    {
        // subtitle streams
        json::JsonArrayStreamWriter subtitle;
        for(std::vector<avtranscoder::SubtitleProperties>::const_iterator it = _subtitleStreams.begin();
            it != _subtitleStreams.end(); ++it)
        {
            subtitle << it->asJson();
        }
        writer << std::make_pair("subtitle", subtitle.build());
    }
    {
        // attachement streams
        json::JsonArrayStreamWriter attachement;
        for(std::vector<avtranscoder::AttachementProperties>::const_iterator it = _attachementStreams.begin();
            it != _attachementStreams.end(); ++it)
        {
            attachement << it->asJson();
        }
        writer << std::make_pair("attachement", attachement.build());
    }
    {
        // unknown streams
        json::JsonArrayStreamWriter unknown;
        for(std::vector<avtranscoder::UnknownProperties>::const_iterator it = _unknownStreams.begin();
            it != _unknownStreams.end(); ++it)
        {
            unknown << it->asJson();
        }
        writer << std::make_pair("unknown", unknown.build());
    }
    return writer.build();
}

std::ostream& operator<<(std::ostream& flux, const FileProperties& fileProperties)
{
    flux << std::left;
    flux << detail::separator << " Wrapper " << detail::separator << std::endl;

    PropertyVector properties = fileProperties.asVector();
    for(PropertyVector::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        flux << std::setw(detail::keyWidth) << it->first << ": " << it->second << std::endl;
    }

    return flux;
}
}
