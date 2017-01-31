#ifndef _AV_TRANSCODER_INPUT_STREAM_DESC_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_DESC_HPP_

#include <AvTranscoder/common.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

/**
 * @brief Structure to describe the source data to extract.
 */
struct InputStreamDesc
{

    InputStreamDesc()
        : _filename()
        , _streamIndex(0)
        , _channelIndexArray()
    {
    }

    InputStreamDesc(const std::string& filename, const size_t streamIndex, const std::vector<size_t>& channelIndexArray)
        : _filename(filename)
        , _streamIndex(streamIndex)
        , _channelIndexArray(channelIndexArray)
    {
    }

    InputStreamDesc(const std::string& filename, const size_t streamIndex, const size_t channelIndex)
        : _filename(filename)
        , _streamIndex(streamIndex)
        , _channelIndexArray()
    {
        _channelIndexArray.push_back(channelIndex);
    }

    /**
     * @brief Read all the channels of the indicated stream.
     */
    InputStreamDesc(const std::string& filename, const size_t streamIndex)
        : _filename(filename)
        , _streamIndex(streamIndex)
        , _channelIndexArray()
    {
    }

    /**
     * @brief Read all the channels of the stream at index 0.
     */
    InputStreamDesc(const std::string& filename)
        : _filename(filename)
        , _streamIndex(0)
        , _channelIndexArray()
    {
    }

    /**
     * @return If a demultiplexing step will be done to extract the expected data.
     */
    bool demultiplexing() const { return !_channelIndexArray.empty(); }

public:
    std::string _filename;                  ///< Source file path.
    size_t _streamIndex;                    ///< Source stream to extract.
    std::vector<size_t> _channelIndexArray; ///< List of source channels to extract from the stream
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const InputStreamDesc& inputStreamDesc);
#endif

}

#endif
