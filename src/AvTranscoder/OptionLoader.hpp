#ifndef _AV_TRANSCODER_OPTION_LOADER_HPP
#define	_AV_TRANSCODER_OPTION_LOADER_HPP

#include <AvTranscoder/Option.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

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
	~OptionLoader();
	
	std::vector<Option>& getOptions() { return m_options; }
	
	const AVFormatContext* getFormatContext() const { return m_avFormatContext; }
	const AVCodecContext* getCodecContext() const { return m_avCodecContext; }
	
	/**
	 * @brief: load array of Option depending on the flags.
     * @param req_flags: AVOption flags we want to load.
     */
	void loadOptions( int req_flags );
	
private:
	std::vector<Option> m_options;
	
	AVFormatContext* m_avFormatContext;
	AVCodecContext* m_avCodecContext;

};

}

#endif
