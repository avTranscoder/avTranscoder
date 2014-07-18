#include "DummyAudio.hpp"

#include <cassert>
#include <cstring>

namespace avtranscoder
{

DummyAudio::DummyAudio( )
	: InputEssence( )
{
}

DummyAudio::~DummyAudio( )
{
}

void DummyAudio::setAudioDesc( const AudioDesc& audioDesc )
{
	_audioDesc = audioDesc;
	
	_frameDesc.setSampleRate  ( _audioDesc.getCodecContext()->sample_rate );
	_frameDesc.setChannels    ( _audioDesc.getCodecContext()->channels );
	_frameDesc.setFps         ( 25.0 );
	_frameDesc.setSampleFormat( _audioDesc.getCodecContext()->sample_fmt );
}

AudioDesc DummyAudio::getAudioDesc() const
{
	return _audioDesc;
}

bool DummyAudio::readNextFrame( Frame& frameBuffer )
{
	frameBuffer.getBuffer().resize( _frameDesc.getDataSize() );

	AudioFrame& audioFrameBuffer = static_cast<AudioFrame&>( frameBuffer );
	audioFrameBuffer.setNbSamples( 1.0 * _frameDesc.getSampleRate() / _frameDesc.getFps() );

	//av_samples_set_silence( data.getPtr(), offset, nb_samples, nb_channels, sample_fmt );
	int fill_char = (
		_frameDesc.getSampleFormat() == AV_SAMPLE_FMT_U8 ||
		_frameDesc.getSampleFormat() == AV_SAMPLE_FMT_U8P
		) ? 0x80 : 0x00;

	memset( frameBuffer.getPtr(), fill_char, frameBuffer.getSize() );

	return true;
}

bool DummyAudio::readNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

}
