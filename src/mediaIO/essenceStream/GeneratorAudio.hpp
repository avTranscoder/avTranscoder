#ifndef _MEDIA_MANAGER_IO_ESSENCE_STREAM_GENERATOR_AUDIO_HPP_
#define _MEDIA_MANAGER_IO_ESSENCE_STREAM_GENERATOR_AUDIO_HPP_

#include "IInputEssence.hpp"
#include <mediaIO/codec/AudioCodec.hpp>

namespace mediamanager
{
namespace mediaio
{

class MediaManagerExport GeneratorAudio : public IInputEssence
{
public:
	GeneratorAudio( );

	void setAudioCodec( const AudioCodec& codec );

	const AudioCodec& getAudioCodec();
	
	void setup() {}
	
	void setFrame( mediacore::Frame& inputFrame );

	bool readNextFrame( mediacore::Frame& frameBuffer );
	bool readNextFrame( mediacore::Frame& frameBuffer, const size_t subStreamIndex );

private:
	const AudioCodec*          _codec;
	mediacore::Frame*          _inputFrame;
	mediacore::AudioFrameDesc  _frameDesc;
};

}
}

#endif