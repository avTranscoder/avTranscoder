#ifndef _MEDIA_ENGINE_CORE_FRAME_AUDIO_FRAME_HPP_
#define _MEDIA_ENGINE_CORE_FRAME_AUDIO_FRAME_HPP_

#include "Frame.hpp"
#include <mediaCore/profile/Profile.hpp>

namespace mediaengine
{
namespace mediacore
{

class MediaEngineExport AudioFrameDesc
{
public:
	AudioFrameDesc()
		: _sampleRate( 0 )
		, _channels( 0 )
		, _fps( 1.0 )
		, _sampleFormat( AV_SAMPLE_FMT_NONE )
	{};

	void setSampleRate  ( const size_t sampleRate ){ _sampleRate = sampleRate; }
	void setChannels    ( const size_t channels   ){ _channels   = channels;   }
	void setFps         ( const size_t fps        ){ _fps        = fps;        }
	void setSampleFormat( const std::string& sampleFormatName ){ _sampleFormat = av_get_sample_fmt( sampleFormatName.c_str() ); }
	void setSampleFormat( const AVSampleFormat sampleFormat ){ _sampleFormat = sampleFormat; }

	size_t getDataSize() const
	{
		return ( _sampleRate / _fps ) * _channels * av_get_bytes_per_sample( _sampleFormat );
	}
	
	void setParameters( const Profile::ProfileDesc& desc )
	{
		if( desc.find( constants::mediaProfileSampleFormat ) != desc.end() )
			setSampleFormat( desc.find( constants::mediaProfileSampleFormat )->second );
	}

	size_t getSampleRate() const { return _sampleRate; }
	size_t getChannels  () const { return _channels; }
	size_t getFps       () const { return _fps; }
	AVSampleFormat getAVSampleFormat() const { return _sampleFormat; }
	std::string getSampleFormat() const
	{
		const char* formatName = av_get_sample_fmt_name( _sampleFormat );
		return formatName ? std::string( formatName ) : "unknown audio sample format";
	}

private:
	size_t _sampleRate;
	size_t _channels;
	double _fps;

	AVSampleFormat _sampleFormat;
};

class MediaEngineExport AudioFrame : public Frame
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
}

#endif
