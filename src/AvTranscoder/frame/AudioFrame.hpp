#ifndef _AV_TRANSCODER_FRAME_AUDIO_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_AUDIO_FRAME_HPP_

#include "Frame.hpp"
#include <AvTranscoder/ProfileLoader.hpp>

extern "C" {
#include <libavutil/samplefmt.h>
}

#include <stdexcept>

namespace avtranscoder
{

class AvExport AudioFrameDesc
{
public:
	AudioFrameDesc( const size_t sampleRate = 0, const size_t channels = 0, const AVSampleFormat sampleFormat = AV_SAMPLE_FMT_NONE )
		: _sampleRate( sampleRate )
		, _channels( channels )
		, _sampleFormat( sampleFormat )
	{}
	AudioFrameDesc( const size_t sampleRate, const size_t channels, const std::string& sampleFormat )
		: _sampleRate( sampleRate )
		, _channels( channels )
		, _sampleFormat( av_get_sample_fmt( sampleFormat.c_str() ) )
	{}

	size_t getSampleRate() const { return _sampleRate; }
	size_t getChannels() const { return _channels; }
	AVSampleFormat getSampleFormat() const { return _sampleFormat; }
	std::string getSampleFormatName() const
	{
		const char* formatName = av_get_sample_fmt_name( _sampleFormat );
		return formatName ? std::string( formatName ) : "unknown sample format";
	}

	size_t getDataSize() const
	{
		if( _sampleFormat == AV_SAMPLE_FMT_NONE )
			throw std::runtime_error( "incorrect sample format" );

		size_t size = _sampleRate * _channels * av_get_bytes_per_sample( _sampleFormat );
		if( size == 0 )
			throw std::runtime_error( "unable to determine audio buffer size" );

		return size;
	}
	
	void setSampleRate( const size_t sampleRate ) { _sampleRate = sampleRate; }
	void setChannels( const size_t channels ) { _channels = channels; }
	void setSampleFormat( const std::string& sampleFormatName ) { _sampleFormat = av_get_sample_fmt( sampleFormatName.c_str() ); }
	void setSampleFormat( const AVSampleFormat sampleFormat ) { _sampleFormat = sampleFormat; }
	
	void setParameters( const ProfileLoader::Profile& profile )
	{
		if( profile.find( constants::avProfileSampleFormat ) != profile.end() )
			setSampleFormat( profile.find( constants::avProfileSampleFormat )->second );
	}

private:
	size_t _sampleRate;
	size_t _channels;
	AVSampleFormat _sampleFormat;
};

class AvExport AudioFrame : public Frame
{
public:
	AudioFrame( const AudioFrameDesc& ref )
		: _audioFrameDesc( ref )
		, _nbSamples( 0 )
	{
		_dataBuffer = DataBuffer( ref.getDataSize(), (unsigned char) 0 );
	}

	const AudioFrameDesc& desc() const    { return _audioFrameDesc; }
	
	size_t getNbSamples() const { return _nbSamples; }
	void setNbSamples( size_t nbSamples ) { _nbSamples = nbSamples; }

private:
	const AudioFrameDesc _audioFrameDesc;
	size_t _nbSamples;
};

}

#endif
