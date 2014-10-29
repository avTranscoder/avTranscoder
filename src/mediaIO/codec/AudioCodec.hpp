#ifndef _MEDIA_ENGINE_IO_CODEC_AUDIO_CODEC_HPP_
#define _MEDIA_ENGINE_IO_CODEC_AUDIO_CODEC_HPP_

#include "ICodec.hpp"
#include <mediaCore/frame/AudioFrame.hpp>

namespace mediaengine
{
namespace mediaio
{

class MediaEngineExport AudioCodec : public ICodec
{
public:
	AudioCodec( const ECodecType type, const std::string& codecName = "" );
	AudioCodec( const ECodecType type, const AVCodecID codecId );
	AudioCodec( const ICodec& codec );

	mediacore::AudioFrameDesc getFrameDesc() const;
	const size_t getSampleRate() const;
	const size_t getChannels() const;
	const AVSampleFormat getAVSampleFormat() const;
	
	void setAudioParameters( const mediacore::AudioFrameDesc& audioFrameDesc );
	void setAudioParameters( const size_t sampleRate, const size_t channels, const AVSampleFormat sampleFormat );
};

}
}

#endif