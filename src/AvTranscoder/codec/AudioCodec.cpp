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

AudioCodec::AudioCodec( const ECodecType type, AVCodecContext& avCodecContext )
	: ICodec( type, avCodecContext )
{
}

AudioFrameDesc AudioCodec::getAudioFrameDesc() const
{
	assert( _avCodecContext != NULL );
	AudioFrameDesc audioFrameDesc( _avCodecContext->sample_rate, _avCodecContext->channels, _avCodecContext->sample_fmt );
	double fps = 1.0 * _avCodecContext->time_base.den / ( _avCodecContext->time_base.num * _avCodecContext->ticks_per_frame );
	if( ! isinf( fps ) )
		audioFrameDesc.setFps( fps );
	return audioFrameDesc;
}

void AudioCodec::setAudioParameters( const AudioFrameDesc& audioFrameDesc )
{
	_avCodecContext->sample_rate = audioFrameDesc.getSampleRate();
	_avCodecContext->channels = audioFrameDesc.getChannels();
	_avCodecContext->sample_fmt = audioFrameDesc.getSampleFormat();
	_avCodecContext->time_base.num = 1;
	_avCodecContext->time_base.den = audioFrameDesc.getFps();
	_avCodecContext->ticks_per_frame = 1;
}

}
