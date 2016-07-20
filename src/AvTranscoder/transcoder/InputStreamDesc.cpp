#include "InputStreamDesc.hpp"

namespace avtranscoder
{

std::ostream& operator<<(std::ostream& flux, const InputStreamDesc& inputStreamDesc)
{
    flux << "input stream description: " << std::endl;
    flux << "- filename: " << inputStreamDesc._filename << std::endl;
    flux << "- stream index: " << inputStreamDesc._streamIndex << std::endl;
    if(inputStreamDesc.demultiplexing())
    {
        flux << "- channels index: ";
        for(size_t c = 0; c < inputStreamDesc._channelIndexArray.size(); ++c)
        {
            flux << inputStreamDesc._channelIndexArray.at(c) << ", ";
        }
        flux << std::endl;
    }
    return flux;
}

}
