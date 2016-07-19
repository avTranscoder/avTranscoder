#ifndef _AV_TRANSCODER_FILTER_FILTER_GRAPH_HPP_
#define _AV_TRANSCODER_FILTER_FILTER_GRAPH_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/filter/Filter.hpp>
#include <AvTranscoder/codec/ICodec.hpp>
#include <AvTranscoder/data/decoded/Frame.hpp>

#include <vector>

struct AVFilterGraph;

namespace avtranscoder
{

/**
 * @brief Manager of filters.
 * @warning Currently, the class manages only filters which has one input and one output.
 * @note See 'complex graph' definition in ffmpeg documentation.
 **/
class AvExport FilterGraph
{
private:
    FilterGraph(const FilterGraph& otherFilterGraph);
    FilterGraph& operator=(const FilterGraph& otherFilterGraph);

public:
    FilterGraph(const ICodec& codec);
    ~FilterGraph();

    /**
     * @brief Add a filter.
     * @param filterName: the method gets the filter definition from this name.
     * @param filterArgs: options to initialize the filter with. This must be a ':'-separated list of options in the
     * 'key=value' form.
     * @param instanceName: name of the instance filter in the graph (if empty, same as filterName).
     * @return the filter added
     * @throw runtime exception if the filter is not found
     * @warning The filter will be added to the filter graph when calling process method.
     * @see process
     */
    Filter& addFilter(const std::string& filterName, const std::string& filterOptions = "",
                      const std::string& instanceName = "");

    /**
     * @brief Pull filtered data from the filter graph, and put result to the given frame.
     * @param inputFrame: input data.
     * @param inputFrame: output data.
     * @note Do nothing if there was no filter added.
     */
    void process(const Frame& inputFrame, Frame& outputFrame);

private:
    /**
     * @return If at least one filter has been added to the filter graph
     */
    bool hasFilters() const { return !_filters.empty(); }

    /**
     * @brief Initialize the graph of filters to process.
     * @see pushFilterToGraph
     * @see pushInBuffer
     * @see pushOutBuffer
     */
    void init(const Frame& inputFrame, Frame& outputFrame);

    /**
     * @brief Push the given Filter to the graph.
     */
    void pushFilter(Filter& filter);

    ///@{
    /// @brief Push the input and output buffer at the beginning and the end of the graph.
    void pushInBuffer(const Frame& frame);
    void pushOutBuffer(const Frame& frame);
    //@}

private:
    AVFilterGraph* _graph;         ///< The graph which holds the filters.
    std::vector<Filter*> _filters; ///< List of filters to process.
    const ICodec& _codec;          ///< Codec of the stream on which the filters will be applied.

    /**
     * @brief Is the FilterGraph initialized.
     * @see init
     */
    bool _isInit;
};
}

#endif
