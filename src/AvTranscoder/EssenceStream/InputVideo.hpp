#ifndef _AV_TRANSCODER_ESSENCE_STREAM_INPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_INPUT_VIDEO_HPP_

#include "InputEssence.hpp"

#include <AvTranscoder/Profile.hpp>

class AVCodec;
class AVCodecContext;
class AVFrame;

namespace avtranscoder
{

class AvInputStream;

class AvExport InputVideo : public InputEssence
{
public:
	InputVideo( AvInputStream& inputStream );
	~InputVideo();
	
	void setup();

	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

	void flushDecoder();
	
	void setProfile( const Profile::ProfileDesc& desc );
	
private:
	AvInputStream*     _inputStream;
	AVCodec*           _codec;
	AVCodecContext*    _codecContext;
	AVFrame*           _frame;

	int                _selectedStream;
};

}

#endif
