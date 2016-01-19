#include "UnknownProperties.hpp"

#include <AvTranscoder/properties/util.hpp>

namespace avtranscoder
{

std::ostream& operator<<(std::ostream& flux, const UnknownProperties& unknownProperties)
{
    flux << detail::separator << " Unknown stream " << detail::separator << std::endl;

    PropertyVector properties = unknownProperties.asVector();
    for(PropertyVector::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        flux << std::setw(detail::keyWidth) << it->first << ": " << it->second << std::endl;
    }

    return flux;
}
}
