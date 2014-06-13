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

#include <string>
#include <vector>
#include <map>
#include <utility> //pair

namespace avtranscoder
{

/**
 * @brief Manage Options.
 * Get a list of options by calling a "load_function", depending on which FFMpeg / libAv object you want to analyse.
 */
class OptionLoader 
{
public:
	typedef std::vector<Option> OptionArray;
	typedef std::map< std::string, std::vector<Option> > OptionMap;
	
public:
	OptionLoader();
	~OptionLoader();
	
	const AVFormatContext* getFormatContext() const { return m_avFormatContext; }
	const AVCodecContext* getCodecContext() const { return m_avCodecContext; }
	const AVOutputFormat* getOutputFormat() const { return m_outputFormat; }
	const AVCodec* getCodec() const { return m_codec; }
	
	/**
     * @param req_flags: AVOption flags (default = 0: no flag restriction)
     */
	OptionArray loadFormatContextOptions( int req_flags = 0 );
	
	/**
     * @param req_flags: AVOption flags (default = 0: no flag restriction)
     */
	OptionArray loadCodecContextOptions( int req_flags = 0 );
	
	OptionMap loadOutputFormatOptions();
private:
	/**
	 * @brief: load array of Option depending on the flags.
     * @param req_flags: AVOption flags we want to load.
     */
	OptionArray loadOptions( void* av_class, int req_flags = 0 );
	
	AVFormatContext* m_avFormatContext;
	AVCodecContext* m_avCodecContext;
	
	AVOutputFormat* m_outputFormat;
	AVCodec* m_codec;

};

}

#endif
