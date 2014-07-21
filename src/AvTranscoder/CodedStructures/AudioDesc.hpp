#ifndef _AV_TRANSCODER_DATA_AUDIO_DESC_HPP_
#define _AV_TRANSCODER_DATA_AUDIO_DESC_HPP_

#include <string>

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/EssenceStructures/AudioFrame.hpp>

#include "CodedDesc.hpp"

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