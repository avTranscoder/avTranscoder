#ifndef _AV_TRANSCODER_OPTION_LOADER_HPP
#define	_AV_TRANSCODER_OPTION_LOADER_HPP

#include <AvTranscoder/Option.hpp>

#include <vector>

namespace avtranscoder
{

/**
 * @brief Manage a list of Option.
 * The list is filled by loading options from a libav / ffmpeg object.
 */
class OptionLoader 
{
public:
	OptionLoader();
	
	std::vector<Option>& getOptions() { return m_options; }
	
	/**
     * @param av_class: a libav / ffmpeg object which contains AVOption.
     */
	void loadOptions( void* av_class, int req_flags, int rej_flags );
	
private:
	std::vector<Option> m_options;
};

}

#endif
