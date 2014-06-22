#ifndef _AV_TRANSCODER_DATA_AUDIO_FRAME_HPP_
#define _AV_TRANSCODER_DATA_AUDIO_FRAME_HPP_

#include <AvTranscoder/common.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#ifndef INT64_C
	#define INT64_C(c) (c ## LL)
	#define UINT64_C(c) (c ## ULL)
#endif
#include <libavcodec/avcodec.h>
}

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

//#include "Sample.hpp"
#include "Frame.hpp"

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
	void setSampleFormat( const AVSampleFormat sampleFormat ){ m_sampleFormat = sampleFormat; }

	size_t getDataSize() const
	{
		return ( m_sampleRate / m_fps ) * m_channels * av_get_bytes_per_sample( m_sampleFormat );
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
