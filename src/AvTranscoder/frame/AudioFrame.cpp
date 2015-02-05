#include "AudioFrame.hpp"

#include <stdexcept>

namespace avtranscoder
{

AudioFrameDesc::AudioFrameDesc( const size_t sampleRate, const size_t channels, const AVSampleFormat sampleFormat )
	: _sampleRate( sampleRate )
	, _channels( channels )
	, _sampleFormat( sampleFormat )
	, _fps( 1. )
{}

AudioFrameDesc::AudioFrameDesc( const size_t sampleRate, const size_t channels, const std::string& sampleFormat )
	: _sampleRate( sampleRate )
	, _channels( channels )
	, _sampleFormat( av_get_sample_fmt( sampleFormat.c_str() ) )
	, _fps( 1. )
{}

std::string AudioFrameDesc::getSampleFormatName() const
{
	const char* formatName = av_get_sample_fmt_name( _sampleFormat );
	return formatName ? std::string( formatName ) : "unknown sample format";
}

size_t AudioFrameDesc::getDataSize() const
{
	if( _sampleFormat == AV_SAMPLE_FMT_NONE )
		throw std::runtime_error( "incorrect sample format" );

	size_t size = ( _sampleRate / _fps ) * _channels * av_get_bytes_per_sample( _sampleFormat );
	if( size == 0 )
		throw std::runtime_error( "unable to determine audio buffer size" );

	return size;
}

void AudioFrameDesc::setSampleFormat( const std::string& sampleFormatName )
{
	_sampleFormat = av_get_sample_fmt( sampleFormatName.c_str() );
}

void AudioFrameDesc::setParameters( const ProfileLoader::Profile& profile )
{
	if( profile.find( constants::avProfileSampleFormat ) != profile.end() )
		setSampleFormat( profile.find( constants::avProfileSampleFormat )->second );
}

}
