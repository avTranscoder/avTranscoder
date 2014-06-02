#include "AvInputStream.hpp"

#include <AvTranscoder/InputFile.hpp>

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

AvInputStream::AvInputStream( )
	: InputStream( )
	, m_inputFile( NULL )
	, m_packetDuration( 0 )
	, m_streamIndex( 0 )
	, m_bufferized( false )
{
}

AvInputStream::AvInputStream( InputFile& inputFile, const size_t streamIndex )
		: InputStream( )
		, m_inputFile( &inputFile )
		, m_packetDuration( 0 )
		, m_streamIndex( streamIndex )
		, m_bufferized( false )
{
}

AvInputStream::~AvInputStream( )
{
}

bool AvInputStream::readNextPacket( DataStream& data )
{
	if( ! m_bufferized )
		throw std::runtime_error( "Can't read packet on non-bufferized input stream." );

	if( m_streamCache.empty() )
		m_inputFile->readNextPacket( m_streamIndex );

	if( m_streamCache.empty() )
		return false;

	m_streamCache.front().getBuffer().swap( data.getBuffer() );

	m_streamCache.erase( m_streamCache.begin() );

	return true;
}

void AvInputStream::addPacket( AVPacket& packet )
{
	//std::cout << "add packet for stream " << m_streamIndex << std::endl;
	DataStream data;
	m_streamCache.push_back( data );
	m_packetDuration = packet.duration;

	if( ! m_bufferized )
		return;

	// is it possible to remove this copy ?
	// using : av_packet_unref ?
	m_streamCache.back().getBuffer().resize( packet.size );
	if( packet.size != 0 )
		memcpy( m_streamCache.back().getPtr(), packet.data, packet.size );

	// std::vector<unsigned char> tmpData( 0,0 );
	// &tmpData[0] = packet.data;
	// tmpData.size( packet.size );

	// remove reference on packet because it's passed to DataStream
	// packet.data = NULL;
 	// packet.size = 0;

	// std::cout << this << " buffer size " << m_streamCache.size() << std::endl;
}

VideoDesc AvInputStream::getVideoDesc() const
{
	assert( m_streamIndex <= m_inputFile->getFormatContext().nb_streams );

	if( m_inputFile->getFormatContext().streams[m_streamIndex]->codec->codec_type != AVMEDIA_TYPE_VIDEO )
	{
		throw std::runtime_error( "unable to get video descriptor on non-video stream" );
	}

	AVCodecContext* codecContext = m_inputFile->getFormatContext().streams[m_streamIndex]->codec;

	VideoDesc desc( codecContext->codec_id );

	desc.setImageParameters( codecContext->width, codecContext->height, codecContext->pix_fmt );
	desc.setTimeBase( codecContext->time_base.num, codecContext->time_base.den );

	return desc;
}

AudioDesc AvInputStream::getAudioDesc() const
{
	assert( m_streamIndex <= m_inputFile->getFormatContext().nb_streams );

	if( m_inputFile->getFormatContext().streams[m_streamIndex]->codec->codec_type != AVMEDIA_TYPE_AUDIO )
	{
		throw std::runtime_error( "unable to get audio descriptor on non-audio stream" );
	}

	AVCodecContext* codecContext = m_inputFile->getFormatContext().streams[m_streamIndex]->codec;

	AudioDesc desc( codecContext->codec_id );

	desc.setAudioParameters( codecContext->sample_rate, codecContext->channels, codecContext->sample_fmt );

	return desc;
}


double AvInputStream::getDuration() const
{
	return 1.0 * m_inputFile->getFormatContext().duration / AV_TIME_BASE;
}

double AvInputStream::getPacketDuration() const
{
	return m_packetDuration * av_q2d( m_inputFile->getFormatContext().streams[m_streamIndex]->time_base );
}

void AvInputStream::clearBuffering()
{
	m_streamCache.clear();
}

}
