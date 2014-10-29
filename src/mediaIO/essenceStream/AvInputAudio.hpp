#ifndef _MEDIA_ENGINE_IO_ESSENCE_STREAM_AV_INPUT_AUDIO_HPP_
#define _MEDIA_ENGINE_IO_ESSENCE_STREAM_AV_INPUT_AUDIO_HPP_

#include "IInputEssence.hpp"
#include <mediaIO/codec/AudioCodec.hpp>

struct AVFrame;

namespace mediaengine
{
namespace mediaio
{

class AvInputStream;

class MediaEngineExport AvInputAudio : public IInputEssence
{
public:
	AvInputAudio( AvInputStream& inputStream );
	~AvInputAudio();

	void setup();

	bool readNextFrame( mediacore::Frame& frameBuffer );
	bool readNextFrame( mediacore::Frame& frameBuffer, const size_t subStreamIndex );

private:
	bool getNextFrame();

	AvInputStream*     _inputStream;
	AudioCodec         _codec;
	AVFrame*           _frame;

	int                _selectedStream;
};

}
}

#endif
