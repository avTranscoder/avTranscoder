#ifndef _AV_TRANSCODER_FILTER_FILTER_GRAPH_HPP_
#define _AV_TRANSCODER_FILTER_FILTER_GRAPH_HPP_

#include <AvTranscoder/common.hpp>

#include <vector>
#include <utility>

struct AVFilterGraph;
struct AVFilter;
struct AVFilterContext;

namespace avtranscoder
{

/**
 * @brief 
 **/
class AvExport FilterGraph
{
private:
	FilterGraph( const FilterGraph& otherFilterGraph );
	FilterGraph& operator=( const FilterGraph& otherFilterGraph );

public:
	typedef std::pair<AVFilter*, AVFilterContext*> Filter;

public:
	FilterGraph();
	~FilterGraph();

	void addFilter( const std::string& filtername );
	Filter& getFirstFilter() { return _filters.at(0); }
	Filter& getLastFilter() { return _filters.at(_filters.size()-1); }

	bool hasFilters() const { return ! _filters.empty(); }

	void connectFilters();

private:
	AVFilterGraph* _graph;
	std::vector< Filter > _filters;
};

}

#endif
