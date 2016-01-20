#include "AttachementProperties.hpp"

#include <AvTranscoder/properties/util.hpp>

namespace avtranscoder
{

std::ostream& operator<<(std::ostream& flux, const AttachementProperties& attachementProperties)
{
    flux << detail::separator << " Attachement stream " << detail::separator << std::endl;

    PropertyVector properties = attachementProperties.asVector();
    for(PropertyVector::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        flux << std::setw(detail::keyWidth) << it->first << ": " << it->second << std::endl;
    }

    return flux;
}
}
