#include "FilterGraph.hpp"

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/data/decoded/AudioFrame.hpp>
#include <AvTranscoder/data/decoded/VideoFrame.hpp>

extern "C" {
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
}

#include <stdexcept>
#include <sstream>

namespace avtranscoder
{

FilterGraph::FilterGraph(const ICodec& codec)
    : _graph(avfilter_graph_alloc())
    , _filters()
    , _codec(codec)
    , _isInit(false)
{
    if(!_graph)
        throw std::runtime_error("Unable to create filter graph: out of memory.");
}

FilterGraph::~FilterGraph()
{
    for(std::vector<Filter*>::iterator it = _filters.begin(); it < _filters.end(); ++it)
    {
        it = _filters.erase(it);
    }
    avfilter_graph_free(&_graph);
}

void FilterGraph::process(const std::vector<Frame*>& inputs, Frame& output)
{
    if(!hasFilters())
    {
        LOG_DEBUG("No filter to process: reference first input frame to the given output.")
        output.copyData(*inputs.at(0));
        return;
    }

    // init filter graph
    if(!_isInit)
        init(inputs, output);

    // setup input frames
    for(size_t index = 0; index < inputs.size(); ++index)
    {
        const int ret = av_buffersrc_write_frame(_filters.at(index)->getAVFilterContext(), &inputs.at(index)->getAVFrame());
        if(ret < 0)
        {
            throw std::runtime_error("Error when adding a frame to the source buffer used to start to process filters: " +
                                     getDescriptionFromErrorCode(ret));
        }
    }

    // pull filtered data from the filter graph
    for(;;)
    {
        const int ret = av_buffersink_get_frame(_filters.at(_filters.size() - 1)->getAVFilterContext(), &output.getAVFrame());
        if(ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
            break;
        if(ret < 0)
        {
            throw std::runtime_error("Error reading buffer from buffersink: " + getDescriptionFromErrorCode(ret));
        }
    }
}

Filter& FilterGraph::addFilter(const std::string& filterName, const std::string& filterOptions,
                               const std::string& instanceName)
{
    LOG_INFO("Add filter " << filterName << " to the graph.")
    Filter* filter = new Filter(filterName, filterOptions, instanceName);
    _filters.push_back(filter);
    return *_filters.back();
}

void FilterGraph::init(const std::vector<Frame*>& inputs, Frame& output)
{
    // push filters to the graph
    addInBuffer(inputs);
    addOutBuffer(output);
    for(size_t i = 0; i < _filters.size(); ++i)
    {
        pushFilter(*_filters.at(i));
    }

    // connect filters
    for(size_t index = 0; index < _filters.size() - 1; ++index)
    {
        size_t indexOfOutputFilterToConnect = index + 1;
        size_t indexOfInputPadOfDestinationFilter = 0;
        // handle cases with several inputs
        if(index < inputs.size())
        {
            indexOfOutputFilterToConnect = inputs.size();
            indexOfInputPadOfDestinationFilter = index;
        }

        LOG_INFO("Connect filter " << _filters.at(index)->getName() << " to filter " << _filters.at(indexOfOutputFilterToConnect)->getName())
        const int err =
            avfilter_link(_filters.at(index)->getAVFilterContext(), 0, _filters.at(indexOfOutputFilterToConnect)->getAVFilterContext(), indexOfInputPadOfDestinationFilter);
        if(err < 0)
        {
            throw std::runtime_error("Error when connecting filters.");
        }
    }

    // configuring the graph
    LOG_INFO("Configuring filter graph.")
    const int err = avfilter_graph_config(_graph, NULL);
    if(err < 0)
    {
        throw std::runtime_error("Error configuring the filter graph: " + getDescriptionFromErrorCode(err));
    }

    _isInit = true;
}

void FilterGraph::pushFilter(Filter& filter)
{
    AVFilterContext* context = NULL;
    const int err = avfilter_graph_create_filter(&context, &filter.getAVFilter(), filter.getInstanceName().c_str(),
                                                 filter.getOptions().c_str(), NULL, _graph);
    filter.setAVFilterContext(context);
    if(err < 0)
    {
        std::string msg("Cannot add filter ");
        msg += filter.getName();
        msg += " (instance=";
        msg += filter.getInstanceName();
        msg += ") to the graph: ";
        msg += getDescriptionFromErrorCode(err);
        throw std::runtime_error(msg);
    }
}

void FilterGraph::addInBuffer(const std::vector<Frame*>& inputs)
{
    for(std::vector<Frame*>::const_reverse_iterator it = inputs.rbegin(); it != inputs.rend(); ++it)
    {
        std::string filterName;
        std::stringstream filterOptions;
        // audio frame
        if((*it)->isAudioFrame())
        {
            filterName = "abuffer";
            const AudioFrame* audioFrame = dynamic_cast<const AudioFrame*>(*it);
            filterOptions << "time_base=" << _codec.getAVCodecContext().time_base.num << "/"
                          << _codec.getAVCodecContext().time_base.den << ":";
            filterOptions << "sample_rate=" << audioFrame->getSampleRate() << ":";
            filterOptions << "sample_fmt=" << getSampleFormatName(audioFrame->getSampleFormat()) << ":";
            filterOptions << "channel_layout=0x" << std::hex << audioFrame->getChannelLayout();
        }
        // video frame
        else if((*it)->isVideoFrame())
        {
            filterName = "buffer";
            const VideoFrame* videoFrame = dynamic_cast<const VideoFrame*>(*it);
            filterOptions << "video_size=" << videoFrame->getWidth() << "x" << videoFrame->getHeight() << ":";
            filterOptions << "pix_fmt=" << getPixelFormatName(videoFrame->getPixelFormat()) << ":";
            filterOptions << "time_base=" << _codec.getAVCodecContext().time_base.num << "/"
                          << _codec.getAVCodecContext().time_base.den << ":";
            filterOptions << "pixel_aspect=" << _codec.getAVCodecContext().sample_aspect_ratio.num << "/"
                          << _codec.getAVCodecContext().sample_aspect_ratio.den;
        }
        // invalid frame
        else
            throw std::runtime_error("Cannot create input buffer of filter graph: the given frame is invalid.");

        // add in buffer
        Filter* in = new Filter(filterName, filterOptions.str(), "in");
        LOG_INFO("Add filter '" << filterName << "' at the beginning of the graph.")
        _filters.insert(_filters.begin(), in);
    }
}

void FilterGraph::addOutBuffer(const Frame& output)
{
    std::string filterName;

    if(output.isAudioFrame())
        filterName = "abuffersink";
    else if(output.isVideoFrame())
        filterName = "buffersink";
    else
        throw std::runtime_error("Cannot create output buffer of filter graph: the given frame is invalid.");

    // add out buffer
    addFilter(filterName, "", "out");
}
}
