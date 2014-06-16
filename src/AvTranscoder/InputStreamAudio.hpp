#ifndef _AV_TRANSCODER_INPUT_STREAM_AUDIO_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_AUDIO_HPP_

#include "InputStreamReader.hpp"
#include "DatasStructures/AudioFrame.hpp"

class AVFormatContext;
class AVCodec;
class AVCodecContext;
class AVFrame;

namespace avtranscoder
{

class AvInputStream;

class AvExport InputStreamAudio : public InputStreamReader
{
public:
	InputStreamAudio( AvInputStream& inputStream );
	~InputStreamAudio();

	void setup();

	bool readNextFrame( Frame& frameBuffer );

private:
	AvInputStream*     m_inputStream;
	AVCodec*           m_codec;
	AVCodecContext*    m_codecContext;
	AVFrame*           m_frame;

	int                m_selectedStream;

private:

};

}

#endif
