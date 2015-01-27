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

AudioGenerator::~AudioGenerator()
{
	delete _silent;
}

void AudioGenerator::setAudioFrameDesc( const AudioFrameDesc& frameDesc )
{
	_frameDesc = frameDesc;
	_frameDesc.setFps( 25. );
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
		AudioFrame& audioBuffer = static_cast<AudioFrame&>( frameBuffer );
		audioBuffer.setNbSamples( _frameDesc.getSampleRate() / _frameDesc.getFps() );

		// Generate the silent only once
		if( ! _silent )
		{
			int fillChar = 0;

			// input of convert
			AudioFrame intermediateBuffer( _frameDesc );
			intermediateBuffer.assign( _frameDesc.getDataSize(), fillChar );
			intermediateBuffer.setNbSamples( audioBuffer.getNbSamples() );

			// output of convert
			_silent = new AudioFrame( audioBuffer.desc() );

			// convert and store the silence
			AudioTransform audioTransform;
			audioTransform.convert( intermediateBuffer, *_silent );
		}
		frameBuffer.refData( *_silent );
	}
	// Take audio frame from _inputFrame
	else
	{
		frameBuffer.refData( _inputFrame->getData(), _inputFrame->getSize() );
	}
	return true;
}

bool AudioGenerator::decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

}
