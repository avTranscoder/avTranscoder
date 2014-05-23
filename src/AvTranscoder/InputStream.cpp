#include "InputStream.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/avstring.h>
}

#include <stdexcept>
#include <cassert>

namespace avtranscoder
{

InputStream::InputStream( const InputFile* inputFile, const size_t streamIndex )
		: m_inputFile( inputFile )
		, m_packetDuration( 0 )
		, m_streamIndex( streamIndex )
{
};

InputStream::~InputStream( )
{
}

bool InputStream::readNextPacket( DataStream& data )
{
	AVPacket packet;
	av_init_packet( &packet );

	if( readNextPacket( packet ) )
	{
		m_packetDuration = packet.duration;

		// is it possible to remove this copy ?
		// using : av_packet_unref ?
		data.getBuffer().resize( packet.size );
		if( packet.size != 0 )
			memcpy( data.getPtr(), packet.data, packet.size );
	}
	else
	{
		data.getBuffer().resize( 0 );
	}

	av_free_packet( &packet );

	return data.getBuffer().size() != 0;
}

bool InputStream::readNextPacket( AVPacket& packet ) const
{
	assert( m_inputFile->getFormatContext() != NULL );
	while( 1 )
	{
		int ret = av_read_frame( m_inputFile->getFormatContext(), &packet );
		if( ret < 0 ) // error or end of file
		{
			return false;
		}

		// We only read one stream and skip others
		if( packet.stream_index == (int)m_streamIndex )
		{
			return true;
		}

		// do not delete these 2 lines
		// need to skip packet, delete this one and re-init for reading the next one
		av_free_packet( &packet );
		av_init_packet( &packet );
	}
}

VideoDesc InputStream::getVideoDesc() const
{
	assert( m_inputFile->getFormatContext() != NULL );
	assert( m_streamIndex <= m_inputFile->getFormatContext()->nb_streams );

	if( m_inputFile->getFormatContext()->streams[m_streamIndex]->codec->codec_type != AVMEDIA_TYPE_VIDEO )
	{
		throw std::runtime_error( "unable to get video descriptor on non-video stream" );
	}

	AVCodecContext* codecContext = m_inputFile->getFormatContext()->streams[m_streamIndex]->codec;

	VideoDesc desc( codecContext->codec_id );

	desc.setImageParameters( codecContext->width, codecContext->height, codecContext->pix_fmt );
	desc.setTimeBase( codecContext->time_base.num, codecContext->time_base.den );

	return desc;
}

AudioDesc InputStream::getAudioDesc() const
{
	assert( m_inputFile->getFormatContext() != NULL );
	assert( m_streamIndex <= m_inputFile->getFormatContext()->nb_streams );

	if( m_inputFile->getFormatContext()->streams[m_streamIndex]->codec->codec_type != AVMEDIA_TYPE_AUDIO )
	{
		throw std::runtime_error( "unable to get audio descriptor on non-audio stream" );
	}

	AVCodecContext* codecContext = m_inputFile->getFormatContext()->streams[m_streamIndex]->codec;

	AudioDesc desc( codecContext->codec_id );

	desc.setAudioParameters( codecContext->sample_rate, codecContext->channels, codecContext->sample_fmt );

	return desc;
}


double InputStream::getDuration() const
{
	return 1.0 * m_inputFile->getFormatContext()->duration / AV_TIME_BASE;
}

double InputStream::getPacketDuration() const
{
	return m_packetDuration * av_q2d( m_inputFile->getFormatContext()->streams[m_streamIndex]->time_base );
}

}