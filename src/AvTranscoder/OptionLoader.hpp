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
	
	std::vector<Option>& getOptions() { return _options; }
	
	const AVFormatContext* getFormatContext() const { return _avFormatContext; }
	const AVCodecContext* getCodecContext() const { return _avCodecContext; }
	
	/**
	 * @brief: load array of Option depending on the flags.
     * @param req_flags
     * @param rej_flags
     */
	void loadOptions( int req_flags, int rej_flags );
	
private:
	std::vector<Option> _options;
	
	AVFormatContext* _avFormatContext;
	AVCodecContext*  _avCodecContext;

};

}

#endif
