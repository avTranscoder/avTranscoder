#include "AudioTransform.hpp"
#include "DatasStructures/AudioFrame.hpp"
#include "common.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#if LIBAVCODEC_VERSION_MAJOR > 54
	#include <libavutil/frame.h>
#endif
}

#include <stdexcept>

namespace avtranscoder
{

AudioTransform::AudioTransform()
	: m_audioConvertContext( NULL )
	, m_isInit    ( false )
{
}

bool AudioTransform::init( const AudioFrame& src, const AudioFrame& dst )
{
	m_audioConvertContext = swr_alloc();
	
	if( !m_audioConvertContext )
	{
		throw std::runtime_error( "unable to create audio convert context" );
	}
	
	swr_alloc_set_opts( m_audioConvertContext,
	                    av_get_default_channel_layout( dst.desc().getChannels() ), dst.desc().getSampleFormat(), av_get_default_channel_layout( dst.desc().getSampleRate() ),
	                    av_get_default_channel_layout( src.desc().getChannels() ), src.desc().getSampleFormat(), av_get_default_channel_layout( src.desc().getSampleRate() ),
	                    0, NULL);
	
	if( swr_init( m_audioConvertContext ) < 0 )
	{
		swr_free( &m_audioConvertContext );
		throw std::runtime_error( "unable to open audio convert context" );
	}
	
	return true;
}

void AudioTransform::convert( const AudioFrame& src, AudioFrame& dst )
{
	if( ! m_isInit )
	{
		m_isInit = init( src, dst );
		m_isInit = true;
	}
		
	if( dst.getSize() != src.getSize() )
		dst.getBuffer().resize( src.getSize(), 0 );

	const unsigned char* srcData = src.getPtr();
	unsigned char* dstData = dst.getPtr();
	
	swr_convert( m_audioConvertContext, &dstData, dst.getSize(), &srcData, src.getSize() );
	
	dst.setNbSamples( src.getNbSamples() );
}

}
