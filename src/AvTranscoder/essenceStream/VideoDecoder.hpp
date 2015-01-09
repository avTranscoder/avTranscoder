#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_

#include "IDecoder.hpp"
#include <AvTranscoder/ProfileLoader.hpp>

struct AVFrame;

namespace avtranscoder
{

class AvInputStream;

class AvExport VideoDecoder : public IDecoder
{
public:
	VideoDecoder( AvInputStream& inputStream );
	~VideoDecoder();
	
	void setup();

	bool decodeNextFrame( Frame& frameBuffer );
	bool decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

	void flushDecoder();
	
	void setProfile( const ProfileLoader::Profile& profile );
	
private:
	bool decodeNextFrame();

private:
	AvInputStream* _inputStream;  ///< Stream from which we read next frames (no ownership, has link)
	AVFrame* _frame;  ///< Libav object to store decoded data (has ownership)
};

}

#endif
