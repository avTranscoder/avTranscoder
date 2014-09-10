#include "AudioDesc.hpp"

#include <cassert>

namespace avtranscoder
{

AudioDesc::AudioDesc( const std::string& codecName )
	: CodedDesc( codecName )
{
}

AudioDesc::AudioDesc( const AVCodecID codecId )
	: CodedDesc( codecId )
{
}

AudioDesc::AudioDesc( const CodedDesc& essenceDesc )
	: CodedDesc( essenceDesc.getCodecId() )
{
	_codec = essenceDesc.getCodec();
	_codecContext = essenceDesc.getCodecContext();
}

AudioFrameDesc AudioDesc::getFrameDesc() const
{
	assert( _codecContext != NULL );
	AudioFrameDesc audioFrameDesc;
	
	audioFrameDesc.setChannels( _codecContext->channels );
	audioFrameDesc.setSampleRate( _codecContext->sample_rate );
	audioFrameDesc.setSampleFormat( _codecContext->sample_fmt );
	// audioFrameDesc.setFps( 25 );
	
	return audioFrameDesc;
}

const size_t AudioDesc::getSampleRate() const
{
	assert( _codecContext != NULL );
	return _codecContext->sample_rate;
}

const size_t AudioDesc::getChannels() const
{
	assert( _codecContext != NULL );
	return _codecContext->channels;
}

const AVSampleFormat AudioDesc::getSampleFormat() const
{
	assert( _codecContext != NULL );
	return _codecContext->sample_fmt;
}

void AudioDesc::setAudioParameters( const AudioFrameDesc& audioFrameDesc )
{
	setAudioParameters( audioFrameDesc.getSampleRate(), audioFrameDesc.getChannels(), audioFrameDesc.getSampleFormat() );
}

void AudioDesc::setAudioParameters( const size_t sampleRate, const size_t channels, const AVSampleFormat sampleFormat )
{
	_codecContext->sample_rate = sampleRate;
	_codecContext->channels    = channels;
	_codecContext->sample_fmt  = sampleFormat;
}

}
