#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_AUDIO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_AUDIO_HPP_

#include "IDecoder.hpp"

struct AVFrame;

namespace avtranscoder
{

class AvInputStream;

class AvExport AudioDecoder : public IDecoder
{
public:
	AudioDecoder( AvInputStream& inputStream );
	~AudioDecoder();

	void setup();

	bool decodeNextFrame( Frame& frameBuffer );
	bool decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

private:
	bool decodeNextFrame();

private:
	AvInputStream* _inputStream;  ///< Stream from which we read next frames (no ownership, has link)
	AVFrame* _frame;  ///< Libav object to store decoded data (has ownership)
};

}

#endif
