#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_AUDIO_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_AUDIO_PROPERTIES_HPP

#include <AvTranscoder/properties/StreamProperties.hpp>

#include <string>

namespace avtranscoder
{

class AvExport AudioProperties : public StreamProperties
{
public:
    AudioProperties(const FormatContext& formatContext, const size_t index);

    std::string getSampleFormatName() const;
    std::string getSampleFormatLongName() const;
    std::string getChannelLayout() const;
    std::string getChannelName() const;
    std::string getChannelDescription() const;

    size_t getSampleRate() const;
    size_t getChannels() const;
    size_t getBitRate() const; ///< 0 if unknown
    size_t getNbSamples() const;

    size_t getTicksPerFrame() const;

#ifndef SWIG
    AVCodecContext& getAVCodecContext() { return *_codecContext; }
#endif

    PropertyVector& fillVector(PropertyVector& data) const;

private:
#ifndef SWIG
    template <typename T>
    void addProperty(PropertyVector& data, const std::string& key, T (AudioProperties::*getter)(void) const) const
    {
        try
        {
            detail::add(data, key, (this->*getter)());
        }
        catch(const std::exception& e)
        {
            detail::add(data, key, detail::propertyValueIfError);
        }
    }
#endif
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const AudioProperties& audioProperties);
#endif
}

#endif
