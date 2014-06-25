#include "AudioTransform.hpp"

#include <AvTranscoder/DatasStructures/AudioFrame.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>

#ifdef AV_RESAMPLE_LIBRARY
 #include <libavresample/avresample.h>
 #define AllocResampleContext avresample_alloc_context
 #define FreeResampleContext avresample_free
 #define InitResampleContext avresample_open
 #define SetSampleFormat av_opt_set_int
#else
 #include <libswresample/swresample.h>
 #define AllocResampleContext swr_alloc
 #define FreeResampleContext swr_free
 #define InitResampleContext swr_init
 #define SetSampleFormat av_opt_set_sample_fmt
#endif

#if LIBAVCODEC_VERSION_MAJOR > 54
	#include <libavutil/frame.h>
#endif
}

#include <stdexcept>

namespace avtranscoder
{

AudioTransform::AudioTransform()
	: EssenceTransform()
	, _audioConvertContext( NULL )
	, _isInit    ( false )
{
}

bool AudioTransform::init( const Frame& src, const Frame& dst )
{
	const AudioFrame& srcAudioFrame = static_cast<const AudioFrame&>( src );
	const AudioFrame& dstAudioFrame = static_cast<const AudioFrame&>( dst );

	_audioConvertContext = AllocResampleContext();

	if( !_audioConvertContext )
	{
		throw std::runtime_error( "unable to create audio convert context" );
	}
	
	av_opt_set_int(  _audioConvertContext, "in_channel_layout",  av_get_default_channel_layout( srcAudioFrame.desc().getChannels() ), 0 );
	av_opt_set_int(  _audioConvertContext, "out_channel_layout", av_get_default_channel_layout( dstAudioFrame.desc().getChannels() ), 0 );
	av_opt_set_int(  _audioConvertContext, "in_sample_rate",     srcAudioFrame.desc().getSampleRate(), 0 );
	av_opt_set_int(  _audioConvertContext, "out_sample_rate",    dstAudioFrame.desc().getSampleRate(), 0 );
	SetSampleFormat( _audioConvertContext, "in_sample_fmt",      srcAudioFrame.desc().getSampleFormat(), 0 );
	SetSampleFormat( _audioConvertContext, "out_sample_fmt",     dstAudioFrame.desc().getSampleFormat(), 0 );
	
	if( InitResampleContext( _audioConvertContext ) < 0 )
	{
		FreeResampleContext( &_audioConvertContext );
		throw std::runtime_error( "unable to open audio convert context" );
	}
	
	return true;
}

void AudioTransform::convert( const Frame& src, Frame& dst )
{
	if( ! _isInit )
	{
		_isInit = init( src, dst );
		_isInit = true;
	}

	const AudioFrame& srcAudioFrame = static_cast<const AudioFrame&>( src );
	AudioFrame& dstAudioFrame = static_cast<AudioFrame&>( dst );
		
	if( dstAudioFrame.getSize() != srcAudioFrame.getSize() )
		dstAudioFrame.getBuffer().resize( srcAudioFrame.getSize(), 0 );

	const unsigned char* srcData = srcAudioFrame.getPtr();
	unsigned char* dstData = dstAudioFrame.getPtr();
	
#ifdef AV_RESAMPLE_LIBRARY
	avresample_convert( _audioConvertContext, (uint8_t**)&dstData, 0, dstAudioFrame.getSize(), (uint8_t**)&srcData, 0, srcAudioFrame.getSize() );
#else
	swr_convert( _audioConvertContext, &dstData, dstAudioFrame.getSize(), &srcData, srcAudioFrame.getSize() );
#endif

	dstAudioFrame.setNbSamples( srcAudioFrame.getNbSamples() );
}

}
