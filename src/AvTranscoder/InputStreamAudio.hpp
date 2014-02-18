#ifndef _AV_TRANSCODER_INPUT_STREAM_AUDIO_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_AUDIO_HPP_

#include "InputStream.hpp"
#include "DatasStructures/AudioFrame.hpp"

namespace avtranscoder
{

class InputStreamAudio
{
public:
	InputStreamAudio( const InputStream& inputStream );
	~InputStreamAudio();

	bool readNextFrame( AudioFrame& audioFrameBuffer );

private:
	const InputStream  m_inputStream;
	AVCodec*           m_codec;
	AVCodecContext*    m_codecContext;
	AVFrame*           m_frame;

	int                m_selectedStream;

private:

};

}

#endif