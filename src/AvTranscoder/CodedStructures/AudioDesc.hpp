#ifndef _AV_TRANSCODER_DATA_AUDIO_DESC_HPP_
#define _AV_TRANSCODER_DATA_AUDIO_DESC_HPP_

#include <AvTranscoder/EssenceStructures/AudioFrame.hpp>
#include "CodedDesc.hpp"
#include <AvTranscoder/common.hpp>

#include <string>

class AVCodec;

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