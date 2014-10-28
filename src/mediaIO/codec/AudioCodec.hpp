#ifndef _MEDIA_MANAGER_IO_CODEC_AUDIO_CODEC_HPP_
#define _MEDIA_MANAGER_IO_CODEC_AUDIO_CODEC_HPP_

#include "ICodec.hpp"
#include <mediaCore/frame/AudioFrame.hpp>

namespace mediamanager
{
namespace mediaio
{

class MediaManagerExport AudioCodec : public ICodec
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