#include "DummyInputStream.hpp"

#include <cassert>

namespace avtranscoder
{

DummyInputStream::DummyInputStream( )
	: InputStream( )
{
}

DummyInputStream::~DummyInputStream( )
{
}

bool DummyInputStream::readNextPacket( DataStream& data )
{
	data.getBuffer().resize( m_frameDesc.getDataSize() );

	//av_samples_set_silence( data.getPtr(), offset, nb_samples, nb_channels, sample_fmt );
	int fill_char = ( m_frameDesc.getSampleFormat() == AV_SAMPLE_FMT_U8 || m_frameDesc.getSampleFormat() == AV_SAMPLE_FMT_U8P) ? 0x80 : 0x00;

	memset( data.getPtr(), fill_char, data.getSize() );

	return true;
}

void DummyInputStream::setVideoDesc( VideoDesc videoDesc )
{
	m_videoDesc = videoDesc;
}

void DummyInputStream::setAudioDesc( AudioDesc audioDesc )
{
	m_audioDesc = audioDesc;
	m_frameDesc.setSampleRate  ( m_audioDesc.getCodecContext()->sample_rate );
	m_frameDesc.setChannels    ( m_audioDesc.getCodecContext()->channels );
	m_frameDesc.setFps         ( 25.0 );
	m_frameDesc.setSampleFormat( m_audioDesc.getCodecContext()->sample_fmt );
}


VideoDesc DummyInputStream::getVideoDesc() const
{
	return m_videoDesc;
}

AudioDesc DummyInputStream::getAudioDesc() const
{
	return m_audioDesc;
}


double DummyInputStream::getDuration() const
{
	return 0;
}

double DummyInputStream::getPacketDuration() const
{
	return 0;
}

void DummyInputStream::clearBuffering()
{
}

}