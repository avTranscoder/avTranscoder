#include "DummyAudio.hpp"

namespace avtranscoder
{

DummyAudio::DummyAudio( )
	: IInputEssence( )
	, _inputFrame( NULL )
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

void DummyAudio::setFrame( Frame& inputFrame )
{
	_inputFrame = &inputFrame;
}

bool DummyAudio::readNextFrame( Frame& frameBuffer )
{
	frameBuffer.getBuffer().resize( _frameDesc.getDataSize() );

	if( ! _inputFrame )
	{
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
	
	if( frameBuffer.getSize() != _inputFrame->getSize() )
		frameBuffer.getBuffer().resize( _inputFrame->getSize() );
	std::memcpy( frameBuffer.getPtr(), _inputFrame->getPtr(), _inputFrame->getSize() );
	return true;
}

bool DummyAudio::readNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

}
