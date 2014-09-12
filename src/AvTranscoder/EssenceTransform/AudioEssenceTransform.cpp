#include "AudioEssenceTransform.hpp"

#include <stdexcept>

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

#include <AvTranscoder/EssenceStructures/AudioFrame.hpp>

namespace avtranscoder
{

AudioEssenceTransform::AudioEssenceTransform()
	: _audioConvertContext( NULL )
	, _isInit    ( false )
{
}

bool AudioEssenceTransform::init( const Frame& srcFrame, const Frame& dstFrame )
{
	const AudioFrame& src = static_cast<const AudioFrame&>( srcFrame );
	const AudioFrame& dst = static_cast<const AudioFrame&>( dstFrame );

	_audioConvertContext = AllocResampleContext();

	if( !_audioConvertContext )
	{
		throw std::runtime_error( "unable to create audio convert context" );
	}
	
	av_opt_set_int(  _audioConvertContext, "in_channel_layout",  av_get_default_channel_layout( src.desc().getChannels() ), 0 );
	av_opt_set_int(  _audioConvertContext, "out_channel_layout", av_get_default_channel_layout( dst.desc().getChannels() ), 0 );
	av_opt_set_int(  _audioConvertContext, "in_sample_rate",     src.desc().getSampleRate(), 0 );
	av_opt_set_int(  _audioConvertContext, "out_sample_rate",    dst.desc().getSampleRate(), 0 );
	SetSampleFormat( _audioConvertContext, "in_sample_fmt",      src.desc().getSampleFormat(), 0 );
	SetSampleFormat( _audioConvertContext, "out_sample_fmt",     dst.desc().getSampleFormat(), 0 );
	
	if( InitResampleContext( _audioConvertContext ) < 0 )
	{
		FreeResampleContext( &_audioConvertContext );
		throw std::runtime_error( "unable to open audio convert context" );
	}
	
	return true;
}

void AudioEssenceTransform::convert( const Frame& srcFrame, Frame& dstFrame )
{
	if( ! _isInit )
		_isInit = init( srcFrame, dstFrame );

	const unsigned char* srcData = srcFrame.getPtr();
	unsigned char* dstData = dstFrame.getPtr();

	int nbOutputSamplesPerChannel;
#ifdef AV_RESAMPLE_LIBRARY
	nbOutputSamplesPerChannel = avresample_convert( _audioConvertContext, (uint8_t**)&dstData, 0, dstFrame.getSize(), (uint8_t**)&srcData, 0, srcFrame.getSize() );
#else
	nbOutputSamplesPerChannel = swr_convert( _audioConvertContext, &dstData, dstFrame.getSize(), &srcData, srcFrame.getSize() );
#endif

	if( nbOutputSamplesPerChannel < 0 )
	{
		throw std::runtime_error( "unable to convert audio samples" );
	}

	int nbOutputSamples = nbOutputSamplesPerChannel * static_cast<const AudioFrame&>( dstFrame ).desc().getChannels();
	
	if( dstFrame.getSize() != nbOutputSamples )
		dstFrame.getBuffer().resize( nbOutputSamples, 0 );
	static_cast<AudioFrame&>( dstFrame ).setNbSamples( static_cast<const AudioFrame&>( srcFrame ).getNbSamples() );
}

}
