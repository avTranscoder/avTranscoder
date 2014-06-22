#include "DummyInputStream.hpp"

#include <cassert>
#include <cstring>

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
	data.getBuffer().resize( _frameDesc.getDataSize() );

	//av_samples_set_silence( data.getPtr(), offset, nb_samples, nb_channels, sample_fmt );
	int fill_char = (
		_frameDesc.getSampleFormat() == AV_SAMPLE_FMT_U8 ||
		_frameDesc.getSampleFormat() == AV_SAMPLE_FMT_U8P
		) ? 0x80 : 0x00;

	memset( data.getPtr(), fill_char, data.getSize() );

	return true;
}

void DummyInputStream::setVideoDesc( VideoDesc videoDesc )
{
	_videoDesc = videoDesc;
}

void DummyInputStream::setAudioDesc( AudioDesc audioDesc )
{
	_audioDesc = audioDesc;
	_frameDesc.setSampleRate  ( _audioDesc.getCodecContext()->sample_rate );
	_frameDesc.setChannels    ( _audioDesc.getCodecContext()->channels );
	_frameDesc.setFps         ( 25.0 );
	_frameDesc.setSampleFormat( _audioDesc.getCodecContext()->sample_fmt );
}


VideoDesc DummyInputStream::getVideoDesc() const
{
	return _videoDesc;
}

AudioDesc DummyInputStream::getAudioDesc() const
{
	return _audioDesc;
}

AVMediaType DummyInputStream::getStreamType() const
{
	//TODO return different type whether it's an audio or video type
	return AVMEDIA_TYPE_AUDIO;
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
