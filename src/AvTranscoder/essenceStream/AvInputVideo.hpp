#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_

#include "IInputEssence.hpp"

#include <AvTranscoder/Profile.hpp>

class AVCodec;
class AVCodecContext;
class AVFrame;

namespace avtranscoder
{

class AvInputStream;

class AvExport AvInputVideo : public IInputEssence
{
public:
	AvInputVideo( AvInputStream& inputStream );
	~AvInputVideo();
	
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
