#include "GeneratorAudio.hpp"

namespace avtranscoder
{

GeneratorAudio::GeneratorAudio( )
	: IInputEssence( )
	, _inputFrame( NULL )
	, _frameDesc()
{
}

void GeneratorAudio::setAudioFrameDesc( const AudioFrameDesc& frameDesc )
{
	_frameDesc = frameDesc;
	_frameDesc.setFps( 25. );
}

void GeneratorAudio::setFrame( Frame& inputFrame )
{
	_inputFrame = &inputFrame;
}

bool GeneratorAudio::readNextFrame( Frame& frameBuffer )
{
	frameBuffer.getBuffer().resize( _frameDesc.getDataSize() );

	if( ! _inputFrame )
	{
		AudioFrame& audioFrameBuffer = static_cast<AudioFrame&>( frameBuffer );
		audioFrameBuffer.setNbSamples( 1.0 * _frameDesc.getSampleRate() / _frameDesc.getFps() );

		//av_samples_set_silence( data.getPtr(), offset, nb_samples, nb_channels, sample_fmt );
		int fill_char = (
			_frameDesc.getAVSampleFormat() == AV_SAMPLE_FMT_U8 ||
			_frameDesc.getAVSampleFormat() == AV_SAMPLE_FMT_U8P
			) ? 0x80 : 0x00;

		memset( frameBuffer.getPtr(), fill_char, frameBuffer.getSize() );
		return true;
	}
	
	if( frameBuffer.getSize() != _inputFrame->getSize() )
		frameBuffer.getBuffer().resize( _inputFrame->getSize() );
	std::memcpy( frameBuffer.getPtr(), _inputFrame->getPtr(), _inputFrame->getSize() );
	return true;
}

bool GeneratorAudio::readNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

}
