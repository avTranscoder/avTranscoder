#include "AudioTransform.hpp"

#include <AvTranscoder/frame/AudioFrame.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>

#ifdef AVTRANSCODER_LIBAV_DEPENDENCY
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
	: _audioConvertContext( NULL )
	, _nbSamplesOfPreviousFrame( 0 )
	, _isInit    ( false )
{
}

AudioTransform::~AudioTransform()
{
	FreeResampleContext( &_audioConvertContext );
}

bool AudioTransform::init( const Frame& srcFrame, const Frame& dstFrame )
{
	_audioConvertContext = AllocResampleContext();
	if( !_audioConvertContext )
	{
		throw std::runtime_error( "unable to create audio convert context" );
	}

	const AudioFrame& src = static_cast<const AudioFrame&>( srcFrame );
	const AudioFrame& dst = static_cast<const AudioFrame&>( dstFrame );

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

void AudioTransform::updateOutputFrame( const size_t nbInputSamples, Frame& dstFrame ) const
{
	AudioFrame& dst = static_cast<AudioFrame&>( dstFrame );

	// resize buffer of output frame
	const int dstSampleSize = av_get_bytes_per_sample( dst.desc().getSampleFormat() );
	const size_t bufferSizeNeeded = nbInputSamples * dst.desc().getChannels() * dstSampleSize;
	dstFrame.resize( bufferSizeNeeded );

	// set nbSamples of output frame
	dst.setNbSamples( nbInputSamples );
}

void AudioTransform::convert( const Frame& srcFrame, Frame& dstFrame )
{
	if( ! _isInit )
		_isInit = init( srcFrame, dstFrame );

	// if number of samples change from previous frame
	const size_t nbSamplesOfCurrentFrame = static_cast<const AudioFrame&>( srcFrame ).getNbSamples();
	if( nbSamplesOfCurrentFrame != _nbSamplesOfPreviousFrame )
	{
		updateOutputFrame( nbSamplesOfCurrentFrame, dstFrame );
		_nbSamplesOfPreviousFrame = nbSamplesOfCurrentFrame;
	}

	const unsigned char* srcData = srcFrame.getData();
	unsigned char* dstData = dstFrame.getData();

	int nbOutputSamplesPerChannel;
#ifdef AVTRANSCODER_LIBAV_DEPENDENCY
	nbOutputSamplesPerChannel = avresample_convert( _audioConvertContext, (uint8_t**)&dstData, 0, nbSamplesOfCurrentFrame, (uint8_t**)&srcData, 0, nbSamplesOfCurrentFrame );
#else
	nbOutputSamplesPerChannel = swr_convert( _audioConvertContext, &dstData, nbSamplesOfCurrentFrame, &srcData, nbSamplesOfCurrentFrame );
#endif

	if( nbOutputSamplesPerChannel < 0 )
	{
		throw std::runtime_error( "unable to convert audio samples" );
	}
}

}
