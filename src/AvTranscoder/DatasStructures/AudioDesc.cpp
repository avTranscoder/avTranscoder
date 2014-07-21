#include "AudioDesc.hpp"

#include <cassert>

namespace avtranscoder
{

AudioDesc::AudioDesc( const std::string& codecName )
	: EssenceDesc( codecName )
{
}

AudioDesc::AudioDesc( const AVCodecID codecId )
	: EssenceDesc( codecId )
{
}

AudioDesc::AudioDesc( EssenceDesc& essenceDesc )
	: EssenceDesc( essenceDesc.getCodecId() )
{
	m_codec = essenceDesc.getCodec();
	m_codecContext = essenceDesc.getCodecContext();
}

AudioFrameDesc AudioDesc::getFrameDesc() const
{
	assert( m_codecContext != NULL );
	AudioFrameDesc audioFrameDesc;
	
	audioFrameDesc.setChannels( m_codecContext->channels );
	audioFrameDesc.setSampleRate( m_codecContext->sample_rate );
	audioFrameDesc.setSampleFormat( m_codecContext->sample_fmt );
	// audioFrameDesc.setFps( 25 );
	
	return audioFrameDesc;
}

const size_t AudioDesc::getSampleRate() const
{
	assert( m_codecContext != NULL );
	return m_codecContext->sample_rate;
}

const size_t AudioDesc::getChannels() const
{
	assert( m_codecContext != NULL );
	return m_codecContext->channels;
}

const AVSampleFormat AudioDesc::getSampleFormat() const
{
	assert( m_codecContext != NULL );
	return m_codecContext->sample_fmt;
}


void AudioDesc::setAudioParameters( const size_t sampleRate, const size_t channels, const AVSampleFormat sampleFormat )
{
	m_codecContext->sample_rate = sampleRate;
	m_codecContext->channels    = channels;
	m_codecContext->sample_fmt  = sampleFormat;
}

}
