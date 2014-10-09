#ifndef _AV_TRANSCODER_CODED_STRUCTURES_AUDIO_DESC_HPP_
#define _AV_TRANSCODER_CODED_STRUCTURES_AUDIO_DESC_HPP_

#include "ICodec.hpp"
#include <AvTranscoder/frame/AudioFrame.hpp>

namespace avtranscoder
{

class AvExport AudioCodec : public ICodec
{
public:
	AudioCodec( const std::string& codecName = "" );
	AudioCodec( const AVCodecID codecId );
	AudioCodec( const ICodec& essenceDesc );

	AudioFrameDesc getFrameDesc() const;
	const size_t getSampleRate() const;
	const size_t getChannels() const;
	const AVSampleFormat getSampleFormat() const;
	
	void setAudioParameters( const AudioFrameDesc& audioFrameDesc );
	void setAudioParameters( const size_t sampleRate, const size_t channels, const AVSampleFormat sampleFormat );
};

}

#endif