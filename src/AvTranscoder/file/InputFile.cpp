#include "InputFile.hpp"

#include <AvTranscoder/properties/util.hpp>
#include <AvTranscoder/properties/VideoProperties.hpp>
#include <AvTranscoder/properties/AudioProperties.hpp>
#include <AvTranscoder/properties/DataProperties.hpp>
#include <AvTranscoder/properties/SubtitleProperties.hpp>
#include <AvTranscoder/properties/AttachementProperties.hpp>
#include <AvTranscoder/properties/UnknownProperties.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>
#include <sstream>

namespace avtranscoder
{

InputFile::InputFile(const std::string& filename)
    : _formatContext(filename, AV_OPT_FLAG_DECODING_PARAM)
    , _properties(NULL)
    , _filename(filename)
    , _inputStreams()
{
    // Fill the FormatContext with the stream information
    _formatContext.findStreamInfo();

    // Get the stream information as properties
    _properties = new FileProperties(_formatContext);

    // Create streams
    for(size_t streamIndex = 0; streamIndex < _formatContext.getNbStreams(); ++streamIndex)
    {
        _inputStreams.push_back(new InputStream(*this, streamIndex));
    }
}

InputFile::~InputFile()
{
    delete _properties;
    for(std::vector<InputStream*>::iterator it = _inputStreams.begin(); it != _inputStreams.end(); ++it)
    {
        delete(*it);
    }
}

void InputFile::analyse(IProgress& progress, const EAnalyseLevel level)
{
    _properties->extractStreamProperties(progress, level);
}

bool InputFile::readNextPacket(CodedData& data, const size_t streamIndex)
{
    bool nextPacketFound = false;
    while(!nextPacketFound)
    {
        const int ret = av_read_frame(&_formatContext.getAVFormatContext(), &data.getAVPacket());
        if(ret < 0) // error or end of file
        {
            LOG_INFO("Stop reading the next frame of file '" << _filename << "', stream " << streamIndex << " (" << getDescriptionFromErrorCode(ret) << ")")
            return false;
        }

        // Add Stream info to the packet
        data.refAVStream(_formatContext.getAVStream(streamIndex));

        // if the packet stream is the expected one
        // return the packet data
        const int packetStreamIndex = data.getAVPacket().stream_index;
        if(packetStreamIndex == (int)streamIndex)
        {
            LOG_DEBUG("Get a packet from stream " << streamIndex)
            nextPacketFound = true;
        }
        // else add the packet data to the stream cache
        else
        {
            _inputStreams.at(packetStreamIndex)->addPacket(data.getAVPacket());
            data.clear();
        }
    }
    return true;
}

bool InputFile::seekAtFrame(const size_t frame, const int flag)
{
    const uint64_t position = frame / getFps() * AV_TIME_BASE;
    return _formatContext.seek(position, flag);
}

bool InputFile::seekAtTime(const double time, const int flag)
{
    const uint64_t position = time * AV_TIME_BASE;
    return _formatContext.seek(position, flag);
}

void InputFile::activateStream(const size_t streamIndex, bool activate)
{
    getStream(streamIndex).activate(activate);
}

InputStream& InputFile::getStream(size_t index)
{
    try
    {
        return *_inputStreams.at(index);
    }
    catch(const std::out_of_range& e)
    {
        std::stringstream msg;
        msg << getFilename();
        msg << " has no stream at index ";
        msg << index;
        throw std::runtime_error(msg.str());
    }
}

double InputFile::getFps()
{
    double fps = 1;
    if(_properties->getNbVideoStreams())
        fps = _properties->getVideoProperties().at(0).getFps();
    return fps;
}

void InputFile::setupUnwrapping(const ProfileLoader::Profile& profile)
{
    // check the given profile
    const bool isValid = ProfileLoader::checkFormatProfile(profile);
    if(!isValid)
    {
        std::string msg("Invalid format profile to setup unwrapping.");
        LOG_ERROR(msg)
        throw std::runtime_error(msg);
    }

    if(!profile.empty())
    {
        LOG_INFO("Setup unwrapping with:\n" << profile)
    }

    for(ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it)
    {
        if((*it).first == constants::avProfileIdentificator || (*it).first == constants::avProfileIdentificatorHuman ||
           (*it).first == constants::avProfileType)
            continue;

        try
        {
            Option& formatOption = _formatContext.getOption((*it).first);
            formatOption.setString((*it).second);
        }
        catch(std::exception& e)
        {
            LOG_WARN("InputFile - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what())
        }
    }
}

std::ostream& operator<<(std::ostream& flux, const InputFile& input)
{
    // wrapper
    flux << input.getProperties();

    // video streams
    for(size_t videoStreamIndex = 0; videoStreamIndex < input.getProperties().getNbVideoStreams(); ++videoStreamIndex)
    {
        flux << input.getProperties().getVideoProperties().at(videoStreamIndex);
    }

    // audio streams
    for(size_t audioStreamIndex = 0; audioStreamIndex < input.getProperties().getNbAudioStreams(); ++audioStreamIndex)
    {
        flux << input.getProperties().getAudioProperties().at(audioStreamIndex);
    }

    // data streams
    for(size_t dataStreamIndex = 0; dataStreamIndex < input.getProperties().getNbDataStreams(); ++dataStreamIndex)
    {
        flux << input.getProperties().getDataProperties().at(dataStreamIndex);
    }

    // subtitle streams
    for(size_t subtitleStreamIndex = 0; subtitleStreamIndex < input.getProperties().getNbSubtitleStreams();
        ++subtitleStreamIndex)
    {
        flux << input.getProperties().getSubtitleProperties().at(subtitleStreamIndex);
    }

    // attachement streams
    for(size_t attachementStreamIndex = 0; attachementStreamIndex < input.getProperties().getNbAttachementStreams();
        ++attachementStreamIndex)
    {
        flux << input.getProperties().getAttachementProperties().at(attachementStreamIndex);
    }

    // unknown streams
    for(size_t unknownStreamIndex = 0; unknownStreamIndex < input.getProperties().getNbUnknownStreams();
        ++unknownStreamIndex)
    {
        flux << input.getProperties().getUnknownProperties().at(unknownStreamIndex);
    }

    return flux;
}
}
