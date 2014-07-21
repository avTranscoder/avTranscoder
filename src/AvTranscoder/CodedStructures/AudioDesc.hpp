#ifndef _AV_TRANSCODER_DATA_AUDIO_DESC_HPP_
#define _AV_TRANSCODER_DATA_AUDIO_DESC_HPP_

#include <string>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#ifndef INT64_C
	#define INT64_C(c) (c ## LL)
	#define UINT64_C(c) (c ## ULL)
#endif
#include <libavcodec/avcodec.h>
}

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/EssenceStructures/AudioFrame.hpp>
#include "CodedDesc.hpp"

namespace avtranscoder
{

class AvExport AudioDesc : public CodedDesc
{
public:
	AudioDesc( const std::string& codecName = "" );
	AudioDesc( const AVCodecID codecId );
	AudioDesc( const CodedDesc& essenceDesc );

	AudioFrameDesc getFrameDesc() const;
	const size_t getSampleRate() const;
	const size_t getChannels() const;
	const AVSampleFormat getSampleFormat() const;
	
	void setAudioParameters( const size_t sampleRate, const size_t channels, const AVSampleFormat sampleFormat );
};

}

#endif