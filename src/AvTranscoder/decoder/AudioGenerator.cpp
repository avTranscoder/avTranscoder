#include "AudioGenerator.hpp"

namespace avtranscoder
{

AudioGenerator::AudioGenerator()
	: _inputFrame( NULL )
	, _frameDesc()
{
}

void AudioGenerator::setAudioFrameDesc( const AudioFrameDesc& frameDesc )
{
	_frameDesc = frameDesc;
}

void AudioGenerator::setFrame( Frame& inputFrame )
{
	_inputFrame = &inputFrame;
}

bool AudioGenerator::decodeNextFrame( Frame& frameBuffer )
{
	if( ! _inputFrame )
	{
		AudioFrame& audioFrameBuffer = static_cast<AudioFrame&>( frameBuffer );
		audioFrameBuffer.setNbSamples( 1.0 * _frameDesc.getSampleRate() / 25. );

		//av_samples_set_silence( data.getPtr(), offset, nb_samples, nb_channels, sample_fmt );
		int fill_char = (
			_frameDesc.getSampleFormat() == AV_SAMPLE_FMT_U8 ||
			_frameDesc.getSampleFormat() == AV_SAMPLE_FMT_U8P
			) ? 0x80 : 0x00;

		if( frameBuffer.getSize() != _frameDesc.getDataSize() )
			frameBuffer.getBuffer().resize( _frameDesc.getDataSize() );
		memset( frameBuffer.getPtr(), fill_char, frameBuffer.getSize() );
	}
	else
	{
		frameBuffer.copyData( _inputFrame->getPtr(), _inputFrame->getSize() );
	}
	return true;
}

bool AudioGenerator::decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

}
