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
	assert( _codecContext != NULL );

	AudioFrameDesc audioFrameDesc( _codecContext->sample_rate, _codecContext->channels, _codecContext->sample_fmt );
	// audioFrameDesc.setFps( 25 );
	
	return audioFrameDesc;
}

void AudioCodec::setAudioParameters( const AudioFrameDesc& audioFrameDesc )
{
	setAudioParameters( audioFrameDesc.getSampleRate(), audioFrameDesc.getChannels(), audioFrameDesc.getAVSampleFormat() );
}

void AudioCodec::setAudioParameters( const size_t sampleRate, const size_t channels, const AVSampleFormat sampleFormat )
{
	_codecContext->sample_rate = sampleRate;
	_codecContext->channels    = channels;
	_codecContext->sample_fmt  = sampleFormat;
}

}
