#include "SubtitleProperties.hpp"

#include <AvTranscoder/properties/util.hpp>

namespace avtranscoder
{

std::ostream& operator<<(std::ostream& flux, const SubtitleProperties& subtitleProperties)
{
    flux << detail::separator << " Subtitle stream " << detail::separator << std::endl;

    PropertyVector properties = subtitleProperties.asVector();
    for(PropertyVector::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        flux << std::setw(detail::keyWidth) << it->first << ": " << it->second << std::endl;
    }

    return flux;
}
}
