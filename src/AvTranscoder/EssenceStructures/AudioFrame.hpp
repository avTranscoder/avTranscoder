#ifndef _AV_TRANSCODER_DATA_AUDIO_FRAME_HPP_
#define _AV_TRANSCODER_DATA_AUDIO_FRAME_HPP_

#include "Frame.hpp"
#include <AvTranscoder/Profile.hpp>
#include <AvTranscoder/common.hpp>

namespace avtranscoder
{

class AvExport AudioFrameDesc
{
public:
	AudioFrameDesc()
		: m_sampleRate( 0 )
		, m_channels( 0 )
		, m_fps( 1.0 )
		, m_sampleFormat( AV_SAMPLE_FMT_NONE )
	{};

	void setSampleRate  ( const size_t sampleRate ){ m_sampleRate = sampleRate; }
	void setChannels    ( const size_t channels   ){ m_channels   = channels;   }
	void setFps         ( const size_t fps        ){ m_fps        = fps;        }
	void setSampleFormat( const std::string& sampleFormatName ){ m_sampleFormat = av_get_sample_fmt( sampleFormatName.c_str() ); }
	void setSampleFormat( const AVSampleFormat sampleFormat ){ m_sampleFormat = sampleFormat; }

	size_t getDataSize() const
	{
		return ( m_sampleRate / m_fps ) * m_channels * av_get_bytes_per_sample( m_sampleFormat );
	}
	
	void setParameters( const Profile::ProfileDesc& desc )
	{
		if( desc.find( Profile::avProfileSampleFormat ) != desc.end() )
			setSampleFormat( desc.find( Profile::avProfileSampleFormat )->second );
	}

	size_t getSampleRate() const { return m_sampleRate; }
	size_t getChannels  () const { return m_channels; }
	size_t getFps       () const { return m_fps; }
	AVSampleFormat getSampleFormat() const { return m_sampleFormat; }

private:
	size_t m_sampleRate;
	size_t m_channels;
	double m_fps;

	AVSampleFormat m_sampleFormat;
};

class AvExport AudioFrame : public Frame
{
public:
	AudioFrame( const AudioFrameDesc& ref )
		: m_audioFrameDesc( ref )
		, m_nbSamples( 0 )
	{
		 m_dataBuffer = DataBuffer( ref.getDataSize(), (unsigned char) 0 );
	}

	const AudioFrameDesc& desc() const    { return m_audioFrameDesc; }
	
	size_t getNbSamples() const { return m_nbSamples; }
	void setNbSamples( size_t nbSamples ) { m_nbSamples = nbSamples; }

private:
	const AudioFrameDesc m_audioFrameDesc;
	size_t m_nbSamples;
};

}

#endif
