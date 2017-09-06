#ifndef _AV_TRANSCODER_FILTER_FILTER_GRAPH_HPP_
#define _AV_TRANSCODER_FILTER_FILTER_GRAPH_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/filter/Filter.hpp>
#include <AvTranscoder/codec/ICodec.hpp>
#include <AvTranscoder/data/decoded/IFrame.hpp>
#include <AvTranscoder/data/decoded/AudioFrame.hpp>

#include <vector>
#include <queue>

struct AVFilterGraph;

namespace avtranscoder
{

/**
 * @brief Filter graph input frame buffer.
 * This FIFO buffer contains IFrame pointers and can deliver specific size frames.
 *
 * @todo Only for audio frame, for the moment. Make it usable with video frames.
 **/
class FrameBuffer
{
public:
    FrameBuffer(const AudioFrameDesc& audioFrameDesc);
    ~FrameBuffer();

    /**
     * @brief Return whether the buffer is empty or not.
     */
    bool isEmpty() const { return _frameQueue.empty() && _totalDataSize == 0; }
    /**
     * @brief Return the total amount of data contained in the frames of the buffer.
     */
    size_t getDataSize() const { return _totalDataSize; }
    /**
     * @brief Return the number of frames contained in the buffer.
     */
    size_t getBufferSize() const { return _frameQueue.size(); }

    /**
     * @brief Push a frame at the end of the buffer.
     */
    void addFrame(IFrame* frame);

    /**
     * @brief Retrieve a IFrame pointer of the specified size, from the beginning of the buffer.
     * If no size is specified, the whole first IFrame pointer is returned.
     */
    IFrame* getFrame(const size_t size = 0);

private:
    void popFrame();

    AudioFrameDesc _audioFrameDesc;

    std::queue<IFrame*> _frameQueue;
    size_t _totalDataSize;
    size_t _positionInFrontFrame;

};

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
     * If there is one input buffer, the filter graph is a chain of effects: input -> filter 1 -> filter 2 -> output.
     * If there is several input buffers, the filter graph is like this:
     *                      input 1 ---|
     *                                 |
     *                               filter 1 -> filter 2 -> output
     *                                 |
     *                      input 2 ---|
     * @warning the output frame must be cleared once it has been used
     * @see the av_buffersink_get_frame function documentation
     */
    void process(const std::vector<IFrame*>& inputs, IFrame& output);

    /**
     * @return If at least one filter has been added to the filter graph
     */
    bool hasFilters() const { return !_filters.empty(); }

    bool hasBufferedFrames();
    bool hasBufferedFrames(const size_t index);

private:
    /**
     * @brief Initialize the graph of filters to process.
     * @see pushFilterToGraph
     * @see pushInBuffer
     * @see pushOutBuffer
     */
    void init(const std::vector<IFrame*>& inputs, IFrame& output);

    /**
     * @brief Push the given Filter to the graph.
     */
    void pushFilter(Filter& filter);

    ///@{
    /// @brief Add the input and output buffers at the beginning and the end of the list of filters.
    void addInBuffer(const std::vector<IFrame*>& inputs);
    void addOutBuffer(const IFrame& output);
    //@}

    /**
     * @brief Return the input frame size if not null, or the available size into the corresponding frame buffer
     */
    size_t getAvailableFrameSize(const std::vector<IFrame*>& inputs, const size_t& index);
    /**
     * @brief Get the minimum size between input frames, or available frame buffers
     */
    size_t getMinInputFrameSize(const std::vector<IFrame*>& inputs);

    bool areInputFrameSizeEqual(const std::vector<IFrame*>& inputs);
    bool areFrameBuffersEmpty();

private:
    AVFilterGraph* _graph;         ///< The graph which holds the filters.
    std::vector<Filter*> _filters; ///< List of filters to process.
    const ICodec& _codec;          ///< Codec of the stream on which the filters will be applied.

    std::vector<FrameBuffer> _inputFramesBuffer;

    /**
     * @brief Is the FilterGraph initialized.
     * @see init
     */
    bool _isInit;
};
}

#endif
