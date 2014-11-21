#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_

#include "IInputEssence.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/ProfileLoader.hpp>

struct AVFrame;

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
	
	void setProfile( const ProfileLoader::Profile& profile );
	
private:
	AvInputStream*     _inputStream;
	VideoCodec _codec;
	AVFrame*           _frame;
};

}

#endif
