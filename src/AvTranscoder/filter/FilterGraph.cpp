#include "FilterGraph.hpp"

extern "C" {
#include <libavfilter/avfilter.h>
}

#include <stdexcept>

namespace avtranscoder
{

FilterGraph::FilterGraph()
    : _graph(avfilter_graph_alloc())
    , _filters()
{
    if(!_graph)
        throw std::runtime_error("Unable to create filter graph: out of memory.");
}

FilterGraph::~FilterGraph()
{
    for(std::vector<Filter>::iterator it = _filters.begin(); it < _filters.end(); ++it)
    {
        avfilter_free(it->second);
    }
    avfilter_graph_free(&_graph);
}

void FilterGraph::addFilter(const std::string& filtername)
{
    AVFilter* filter = avfilter_get_by_name(filtername.c_str());
    if(filter)
    {
        AVFilterContext* context = NULL;
        const int err = avfilter_graph_create_filter(&context, filter, NULL, filtername.c_str(), NULL, _graph);
        if(err < 0)
        {
            std::string msg("Cannot add filter ");
            msg += filtername;
            msg += " to the graph: ";
            msg += getDescriptionFromErrorCode(err);
            throw std::runtime_error(msg);
        }
        else
            _filters.push_back(std::make_pair(filter, context));
    }
    else
    {
        throw std::runtime_error("Cannot find filter " + filtername);
    }
}

void FilterGraph::connectFilters()
{
    // connecting filters
    for(size_t index = 0; index < _filters.size() + 1; index += 2)
    {
        const int err = avfilter_link(_filters.at(index).second, 0, _filters.at(index + 1).second, 0);
        if(err < 0)
        {
            throw std::runtime_error("Error connecting filters.");
        }
    }
    // configuring
    const int err = avfilter_graph_config(_graph, NULL);
    if(err < 0)
        throw std::runtime_error("Error configuring the filter graph.");
}
}
