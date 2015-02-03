#include "AudioGenerator.hpp"

#include <AvTranscoder/transform/AudioTransform.hpp>

namespace avtranscoder
{

AudioGenerator::AudioGenerator()
	: _inputFrame( NULL )
	, _silent( NULL )
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
	// Generate silent
	if( ! _inputFrame )
	{
		// Generate the silent only once
		if( ! _silent )
		{
			int fillChar = 0;

			AudioFrame intermediateBuffer( _frameDesc );
			intermediateBuffer.resize( _frameDesc.getDataSize() );
			memset( intermediateBuffer.getPtr(), fillChar, _frameDesc.getDataSize() );

			AudioTransform audioTransform;
			audioTransform.convert( intermediateBuffer, frameBuffer );

			AudioFrame& audioBuffer = static_cast<AudioFrame&>( frameBuffer );
			audioBuffer.setNbSamples( 1.0 * _frameDesc.getSampleRate() / _frameDesc.getFps() );
			_silent = new AudioFrame( audioBuffer.desc() );
			_silent->copyData( audioBuffer.getPtr(), audioBuffer.getSize() );
		}
		frameBuffer = *_silent;
	}
	// Take audio frame from _inputFrame
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
