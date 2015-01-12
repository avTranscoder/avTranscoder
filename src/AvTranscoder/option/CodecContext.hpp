#ifndef _AV_TRANSCODER_CODEC_CONTEXT_HPP_
#define _AV_TRANSCODER_CODEC_CONTEXT_HPP_

#include "Option.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace avtranscoder
{

/**
 * @brief Wrapper of an AVCodecContext.
 */
class AvExport  CodecContext
{
public:
	CodecContext( AVCodec& avCodec, int req_flags = 0 );  ///< Allocate an AVCodecContext with the given AVCodec
	CodecContext( int req_flags = 0 );  ///< Allocate an AVCodecContext with default values
	~CodecContext();
	
	OptionArray getOptions();  ///< Get options as array
	OptionMap& getOptionsMap() { return _options; }  ///< Get options as map

	Option& getOption( const std::string& optionName ) { return _options.at(optionName); }

#ifndef SWIG
	AVCodecContext& getAVCodecContext() const { return *_avCodecContext; }
#endif

private:
	AVCodecContext* _avCodecContext;
	OptionMap _options;
};

}

#endif
