#include "AudioCodec.hpp"

#include <cassert>

namespace avtranscoder
{

AudioCodec::AudioCodec( const ECodecType type, const std::string& codecName )
	: ICodec( type, codecName )
{
}

AudioCodec::AudioCodec( const ECodecType type, const AVCodecID codecId )
	: ICodec( type, codecId )
{
}

AudioCodec::AudioCodec( const ICodec& codec )
	: ICodec( codec )
{
}

AudioFrameDesc AudioCodec::getAudioFrameDesc() const
{
	assert( _avCodecContext != NULL );
	AudioFrameDesc audioFrameDesc( _avCodecContext->sample_rate, _avCodecContext->channels, _avCodecContext->sample_fmt );
	// audioFrameDesc.setFps( 25 );
	return audioFrameDesc;
}

void AudioCodec::setAudioParameters( const AudioFrameDesc& audioFrameDesc )
{
	setAudioParameters( audioFrameDesc.getSampleRate(), audioFrameDesc.getChannels(), audioFrameDesc.getSampleFormat() );
}

void AudioCodec::setAudioParameters( const size_t sampleRate, const size_t channels, const AVSampleFormat sampleFormat )
{
	_avCodecContext->sample_rate = sampleRate;
	_avCodecContext->channels = channels;
	_avCodecContext->sample_fmt = sampleFormat;
}

}
