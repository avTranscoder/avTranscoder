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
     * @param inputs: input data buffers (at least one).
     * @param output: output data buffer.
     * @note Do nothing if there was no filter added.
     *  If there is one input buffer, the filter graph is a chain of effects: input -> filter 1 -> filter 2 -> output.
     * If there is several input buffers, the filter graph is like this:
     *                      input 1 ---|
     *                                 |
     *                               filter 1 -> filter 2 -> output
     *                                 |
     *                      input 2 ---|
     */
    void process(const std::vector<Frame*>& inputs, Frame& output);

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
    void init(const std::vector<Frame*>& inputs, Frame& output);

    /**
     * @brief Push the given Filter to the graph.
     */
    void pushFilter(Filter& filter);

    ///@{
    /// @brief Push the input and output buffer at the beginning and the end of the graph.
    void pushInBuffer(const std::vector<Frame*>& inputs);
    void pushOutBuffer(const Frame& output);
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
