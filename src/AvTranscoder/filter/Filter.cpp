#include "Filter.hpp"

extern "C" {
#include <libavfilter/avfilter.h>
}

#include <stdexcept>

namespace avtranscoder
{

Filter::Filter(const std::string& name, const std::string& options, const std::string& instanceName)
    : _filter(NULL)
    , _context(NULL)
    , _options(options)
    , _instanceName(instanceName.empty() ? name : instanceName)
{
    _filter = (AVFilter*)avfilter_get_by_name(name.c_str());
    if(!_filter)
    {
        std::string msg("Cannot find filter ");
        msg += name;
        msg += ". It will not be added to the filter graph.";
        throw std::runtime_error(msg);
    }
}

Filter::~Filter()
{
    avfilter_free(_context);
}

std::string Filter::getName() const
{
    return _filter->name ? std::string(_filter->name) : "";
}
}
