#ifndef _AV_TRANSCODER_FILTER_FILTER_HPP_
#define _AV_TRANSCODER_FILTER_FILTER_HPP_

#include <AvTranscoder/common.hpp>

struct AVFilter;
struct AVFilterContext;

namespace avtranscoder
{

/**
 * @brief Describe a filter and its options.
 **/
class AvExport Filter
{
public:
    Filter(const std::string& name, const std::string& options = "", const std::string& instanceName = "");
    ~Filter();

    std::string getName() const;
    std::string getOptions() const { return _options; }
    std::string getInstanceName() const { return _instanceName; }

#ifndef SWIG
    AVFilter& getAVFilter() { return *_filter; }
    AVFilterContext* getAVFilterContext() { return _context; }

    void setAVFilterContext(AVFilterContext* newContext) { _context = newContext; }
#endif

private:
    AVFilter* _filter;  ///< Has ownership
    AVFilterContext* _context;  ///< Has link (no ownership)
    std::string _options;
    std::string _instanceName;
};
}

#endif
