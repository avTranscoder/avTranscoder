#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_

#include "IInputEssence.hpp"
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

	bool decodeNextFrame( Frame& frameBuffer );
	bool decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

	void flushDecoder();
	
	void setProfile( const ProfileLoader::Profile& profile );
	void setVerbose( bool verbose = true ) { _verbose = verbose; }
	
private:
	bool decodeNextFrame();

private:
	AvInputStream* _inputStream;  ///< Stream from which we read next frames (no ownership, has link)
	AVFrame* _frame;  ///< Libav object to store decoded data (has ownership)

	bool _verbose;
};

}

#endif
