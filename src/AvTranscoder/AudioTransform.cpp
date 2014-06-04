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
	
	av_opt_set_int(m_audioConvertContext, "in_channel_layout", av_get_default_channel_layout( src.desc().getChannels() ), 0);
	av_opt_set_int(m_audioConvertContext, "out_channel_layout", av_get_default_channel_layout( dst.desc().getChannels() ), 0);
	
	av_opt_set_int(m_audioConvertContext, "in_sample_rate", src.desc().getSampleRate(), 0);
	av_opt_set_int(m_audioConvertContext, "out_sample_rate", dst.desc().getSampleRate(), 0);
	
	av_opt_set_int(m_audioConvertContext, "in_sample_fmt", src.desc().getSampleFormat(), 0);
	av_opt_set_int(m_audioConvertContext, "out_sample_fmt", dst.desc().getSampleFormat(), 0);
	
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
